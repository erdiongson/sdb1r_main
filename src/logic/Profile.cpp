#include "Profile.h"
#include <EEPROM.h>
#include "../gpu/Platform.h"
#include "../Utils.h"
#include "../Constants.h"
#include "Profile.h"
#include "../views/common/Keyboards.h"
#include "../views/ViewCommon.h"
#include "TrayPositionHandler.h"

const int PROFILE_SIZE = sizeof(Profile);
static_assert(PROFILE_SIZE <= RESERVED_PROFILE_SIZE, "Profile size exceeds reserved space");
static_assert(EEPROM_SIZE >= MAX_PROFILES * RESERVED_PROFILE_SIZE, "EEPROM size is too small");

void ProfileManager::blankEEPROM(void) {
  for (int i = 0; i < EEPROM_SIZE; i++)
    EEPROM.put(i, 0);
}

void ProfileManager::preLoadEEPROM(void) {
  for (int i = 0; i < MAX_PROFILES; i++) {
    snprintf(currentProfile.profile_name, sizeof(currentProfile.profile_name), "profile %d", i + 1);

    currentProfile.tube_no_x = TUBES_X_MAX;
    currentProfile.tube_no_y = TUBES_Y_MAX;
    currentProfile.pitch_x = PITCH_DEFAULT;
    currentProfile.pitch_y = PITCH_DEFAULT;
    currentProfile.tray_origin_x = ORIGIN_DEFAULT;
    currentProfile.tray_origin_y = ORIGIN_DEFAULT;
    currentProfile.cycles = CYCLES_DEFAULT;
    currentProfile.vibration_level = VIBRATION_LEVEL_DEFAULT;
    currentProfile.password_enabled = true;
    currentProfile.vibration_duration = VIBRATION_DURATION_DEFAULT;
    currentProfile.z_dip = Z_DIP_DEFAULT;
    currentProfile.staggered = false;
    currentProfile.skip_count = 0;
    for (int j = 0; j < MAX_SKIP_POSITIONS_TOTAL; j++) {
      currentProfile.skip_positions[j] = SkipPosition(0, 0);
    }

    writeProfileEEPROM(i);
  }

  writeCurIDEEPROM(0);  // reset current profile in eeprom to 0
  readProfileEEPROM(0);  // reload profile 0 into memory

  setCurrentProfileNum(0); // set current profile to 0
}

void ProfileManager::checkProfile(void) {
  if (currentProfile.cycles > CYCLES_MAX) currentProfile.cycles = CYCLES_MAX;
  if (currentProfile.cycles < CYCLES_MIN) currentProfile.cycles = CYCLES_MIN;
  if (currentProfile.pitch_x > PITCH_X_MAX) currentProfile.pitch_x = PITCH_X_MAX;
  if (currentProfile.pitch_x < PITCH_X_MIN) currentProfile.pitch_x = PITCH_X_MIN;
  if (currentProfile.pitch_y > PITCH_Y_MAX) currentProfile.pitch_y = PITCH_Y_MAX;
  if (currentProfile.pitch_y < PITCH_Y_MIN) currentProfile.pitch_y = PITCH_Y_MIN;
  if (currentProfile.tray_origin_x > ORIGIN_X_MAX) currentProfile.tray_origin_x = ORIGIN_X_MAX;
  if (currentProfile.tray_origin_x < 0) currentProfile.tray_origin_x = 0;
  if (currentProfile.tray_origin_y > ORIGIN_Y_MAX) currentProfile.tray_origin_y = ORIGIN_Y_MAX;
  if (currentProfile.tray_origin_y < 0) currentProfile.tray_origin_y = 0;
  if (currentProfile.tube_no_x > TUBES_X_MAX) currentProfile.tube_no_x = TUBES_X_MAX;
  if (currentProfile.tube_no_x < TUBES_X_MIN) currentProfile.tube_no_x = TUBES_X_MIN;
  if (currentProfile.tube_no_y > TUBES_Y_MAX) currentProfile.tube_no_y = TUBES_Y_MAX;
  if (currentProfile.tube_no_y < TUBES_Y_MIN) currentProfile.tube_no_y = TUBES_Y_MIN;
  if (currentProfile.z_dip < Z_DIP_MIN) currentProfile.z_dip = Z_DIP_MIN;
  if (currentProfile.z_dip > Z_DIP_MAX) currentProfile.z_dip = Z_DIP_MAX;
}

uint8_t ProfileManager::loadProfile(void) {
  snprintf(g_log_buffer, sizeof(g_log_buffer), "ProfSz:%d", PROFILE_SIZE);
  Logger::log(g_log_buffer);

  this->currentProfileIndex = readCurIDEEPROM();
  if (this->currentProfileIndex >= MAX_PROFILES)
    this->currentProfileIndex = 0;  // if corrcupt data from eeprom, set id=0

  readProfileEEPROM(this->currentProfileIndex);  // reread as it is not profile id 0
  if (currentProfile.cycles > CYCLES_MAX) currentProfile.cycles = CYCLES_MAX;
  if (currentProfile.pitch_x > PITCH_X_MAX) currentProfile.pitch_x = PITCH_X_MAX;
  if (currentProfile.pitch_y > PITCH_Y_MAX) currentProfile.pitch_y = PITCH_Y_MAX;
  if (currentProfile.tray_origin_x > ORIGIN_X_MAX) currentProfile.tray_origin_x = ORIGIN_X_MAX;
  if (currentProfile.tray_origin_y > ORIGIN_Y_MAX) currentProfile.tray_origin_y = ORIGIN_Y_MAX;
  if (currentProfile.tube_no_x > TUBES_X_MAX) currentProfile.tube_no_x = TUBES_X_MAX;
  if (currentProfile.tube_no_y > TUBES_Y_MAX) currentProfile.tube_no_y = TUBES_Y_MAX;
  if (currentProfile.z_dip < Z_DIP_MIN) currentProfile.z_dip = Z_DIP_MIN;
  if (currentProfile.z_dip > Z_DIP_MAX) currentProfile.z_dip = Z_DIP_MAX;

  return this->currentProfileIndex;
}

void ProfileManager::readPassEEPROM(char* pass) {
  int addr = PASSWORD_EEPROM_ADDR;
  char c;
  int i = 0;
  do {
    EEPROM.get(addr++, c);  // Read one character from EEPROM
    if (i < PASSWORD_MAX_LEN - 1) {  // Bounds check
      pass[i++] = c;          // Store the character in pass and increment the index
    }
  } while (c != '\0' && i < PASSWORD_MAX_LEN - 1);  // Repeat until null terminator or max length
  pass[i] = '\0';  // Ensure null termination
}

void ProfileManager::writePassEEPROM(char* pass) {
  int addr = PASSWORD_EEPROM_ADDR;
  int i = 0;
  char c;
  // Ensure password is null-terminated and within bounds
  pass[PASSWORD_MAX_LEN - 1] = '\0';
  do {
    c = pass[i++];          // Get the next character from the pass array and increment the index
    EEPROM.put(addr++, c);  // Write the character to EEPROM
  } while (c != '\0' && i < PASSWORD_MAX_LEN);  // Repeat until null terminator or max length
}

void ProfileManager::writeCurIDEEPROM(uint8_t curprofid) {
  EEPROM.put(PROFILE_START_ADDR, curprofid);
}

uint8_t ProfileManager::readCurIDEEPROM(void) {
  uint8_t temp;

  EEPROM.get(PROFILE_START_ADDR, temp);
  if (temp > MAX_PROFILES) temp = 0;
  return temp;
}

void ProfileManager::writeProfileEEPROM(int index) {
  int address = (index * RESERVED_PROFILE_SIZE) + sizeof(uint8_t);

  // Ensure profile_name is null-terminated before writing
  currentProfile.profile_name[PROFILE_NAME_MAX_LEN - 1] = '\0';
  EEPROM.put(address, currentProfile.profile_name);
  address += sizeof(currentProfile.profile_name);
  EEPROM.put(address, currentProfile.tube_no_x);
  address += sizeof(currentProfile.tube_no_x);
  EEPROM.put(address, currentProfile.tube_no_y);
  address += sizeof(currentProfile.tube_no_y);
  EEPROM.put(address, currentProfile.pitch_x);
  address += sizeof(currentProfile.pitch_x);
  EEPROM.put(address, currentProfile.pitch_y);
  address += sizeof(currentProfile.pitch_y);
  EEPROM.put(address, currentProfile.tray_origin_x);
  address += sizeof(currentProfile.tray_origin_x);
  EEPROM.put(address, currentProfile.tray_origin_y);
  address += sizeof(currentProfile.tray_origin_y);
  EEPROM.put(address, currentProfile.cycles);
  address += sizeof(currentProfile.cycles);
  EEPROM.put(address, currentProfile.vibration_level);
  address += sizeof(currentProfile.vibration_level);
  EEPROM.put(address, currentProfile.password_enabled);
  address += sizeof(currentProfile.password_enabled);
  EEPROM.put(address, currentProfile.vibration_duration);
  address += sizeof(currentProfile.vibration_duration);
  EEPROM.put(address, currentProfile.z_dip);
  address += sizeof(currentProfile.z_dip);
  EEPROM.put(address, currentProfile.staggered);
  address += sizeof(currentProfile.staggered);
  EEPROM.put(address, currentProfile.skip_positions);
  address += sizeof(currentProfile.skip_positions);
  EEPROM.put(address, currentProfile.skip_count);
  address += sizeof(currentProfile.skip_count);
}

void ProfileManager::readProfileEEPROM(int index) {
  int address = (index * RESERVED_PROFILE_SIZE) + sizeof(uint8_t);

  EEPROM.get(address, currentProfile.profile_name);
  // Ensure profile_name is null-terminated after reading
  currentProfile.profile_name[PROFILE_NAME_MAX_LEN - 1] = '\0';
  address += sizeof(currentProfile.profile_name);
  EEPROM.get(address, currentProfile.tube_no_x);
  address += sizeof(currentProfile.tube_no_x);
  EEPROM.get(address, currentProfile.tube_no_y);
  address += sizeof(currentProfile.tube_no_y);
  EEPROM.get(address, currentProfile.pitch_x);
  address += sizeof(currentProfile.pitch_x);
  EEPROM.get(address, currentProfile.pitch_y);
  address += sizeof(currentProfile.pitch_y);
  EEPROM.get(address, currentProfile.tray_origin_x);
  address += sizeof(currentProfile.tray_origin_x);
  EEPROM.get(address, currentProfile.tray_origin_y);
  address += sizeof(currentProfile.tray_origin_y);
  EEPROM.get(address, currentProfile.cycles);
  address += sizeof(currentProfile.cycles);
  EEPROM.get(address, currentProfile.vibration_level);
  address += sizeof(currentProfile.vibration_level);
  EEPROM.get(address, currentProfile.password_enabled);
  address += sizeof(currentProfile.password_enabled);
  EEPROM.get(address, currentProfile.vibration_duration);
  address += sizeof(currentProfile.vibration_duration);
  EEPROM.get(address, currentProfile.z_dip);
  address += sizeof(currentProfile.z_dip);
  EEPROM.get(address, currentProfile.staggered);
  address += sizeof(currentProfile.staggered);
  EEPROM.get(address, currentProfile.skip_positions);
  address += sizeof(currentProfile.skip_positions);
  EEPROM.get(address, currentProfile.skip_count);
  address += sizeof(currentProfile.skip_count);

  checkProfile();
}

// Reads a preview profile from EEPROM at the specified index (without skip data).
// @param index The profile index.
void ProfileManager::readPreviewProfileEEPROM(int index) {
  int address = (index * RESERVED_PROFILE_SIZE) + sizeof(uint8_t);

  EEPROM.get(address, previewProfile.profile_name);
  // Ensure profile_name is null-terminated after reading
  previewProfile.profile_name[PROFILE_NAME_MAX_LEN - 1] = '\0';
  address += sizeof(previewProfile.profile_name);
  EEPROM.get(address, previewProfile.tube_no_x);
  address += sizeof(previewProfile.tube_no_x);
  EEPROM.get(address, previewProfile.tube_no_y);
  address += sizeof(previewProfile.tube_no_y);
  EEPROM.get(address, previewProfile.pitch_x);
  address += sizeof(previewProfile.pitch_x);
  EEPROM.get(address, previewProfile.pitch_y);
  address += sizeof(previewProfile.pitch_y);
  EEPROM.get(address, previewProfile.tray_origin_x);
  address += sizeof(previewProfile.tray_origin_x);
  EEPROM.get(address, previewProfile.tray_origin_y);
  address += sizeof(previewProfile.tray_origin_y);
  EEPROM.get(address, previewProfile.cycles);
  address += sizeof(previewProfile.cycles);
  EEPROM.get(address, previewProfile.vibration_level);
  address += sizeof(previewProfile.vibration_level);
  EEPROM.get(address, previewProfile.password_enabled);
  address += sizeof(previewProfile.password_enabled);
  EEPROM.get(address, previewProfile.vibration_duration);
  address += sizeof(previewProfile.vibration_duration);
  EEPROM.get(address, previewProfile.z_dip);
  address += sizeof(previewProfile.z_dip);
  EEPROM.get(address, previewProfile.staggered);
  address += sizeof(previewProfile.staggered);
  // Skip reading skip_positions and skip_count to save memory

  previewProfileIndex = index;
}

// Verifies the password by prompting the user for input.
// @param phost GPU context for displaying the keyboard.
// @return PasswordVerificationResult indicating success, incorrect, or cancelled.
PasswordVerificationResult ProfileManager::verifyPassword(Gpu_Hal_Context_t* phost) {
  char current_password[PASSWORD_MAX_LEN] = "";
  char input_password[PASSWORD_MAX_LEN] = "";

  readPassEEPROM(current_password);
  if (strcmp(current_password, "") == 0) {
    // Safe copy with bounds checking
    strncpy_P(current_password, INITIAL_PASSWORD, PASSWORD_MAX_LEN - 1);
    current_password[PASSWORD_MAX_LEN - 1] = '\0';
  }

  KeyboardResult kb_result = getKeyboardValue(phost, input_password, "Enter Password", FALSE, PASSWORD_MAX_LEN, NULL);

  // Cancelled
  if (kb_result.action == ACTION_BACK || strcmp(input_password, "") == 0) {
    return PASSWORD_CANCELLED;
  }

  // Attempted
  bool password_valid = (strcmp(current_password, input_password) == 0 || strcmp_P(input_password, SUPER_PASSWORD) == 0);
  if (!password_valid) {
    return PASSWORD_INCORRECT;
  }

  return PASSWORD_SUCCESS;
}

// Gets a reference to the current profile.
// @return Reference to the current profile.
Profile& ProfileManager::getCurrentProfile(void) {
  return currentProfile;
}

// Sets the current profile.
// @param profile The profile to set as current.
void ProfileManager::setCurrentProfile(const Profile& profile) {
  currentProfile = profile;
  checkProfile();  // Validate after setting
}

// Gets the current profile number/ID.
// @return The current profile ID.
uint8_t ProfileManager::getCurrentProfileNum(void) {
  return currentProfileIndex;
}

// Sets the current profile number/ID.
// @param profile_num The profile number to set as current.
void ProfileManager::setCurrentProfileNum(uint8_t profile_num) {
  if (profile_num < MAX_PROFILES) {
    currentProfileIndex = profile_num;
  }
}

// Gets skip data as char strings for UI display.
// @param out_skip_col Output buffer for column skip string (must be at least SKIP_STRING_LEN).
// @param out_skip_row Output buffer for row skip string (must be at least SKIP_STRING_LEN).
// @param out_skip_single_pos Output buffer for individual position skip string (must be at least SKIP_STRING_LEN).
void ProfileManager::getSkipStrings(char* out_skip_col, char* out_skip_row, char* out_skip_single_pos) {
  SkipUtils::convertToStrings(currentProfile.skip_positions, currentProfile.skip_count,
                              out_skip_col, out_skip_row, out_skip_single_pos);
}

// Sets skip data from char strings (from UI).
// @param skip_col Column skip string (format: "C1,C9,...").
// @param skip_row Row skip string (format: "R1,R9,...").
// @param skip_single_pos Individual position skip string (format: "C2R4,C3R4,...").
void ProfileManager::setSkipStrings(const char* skip_col, const char* skip_row, const char* skip_single_pos) {
  currentProfile.skip_count = SkipUtils::convertFromStrings(skip_col, skip_row, skip_single_pos,
                                                             currentProfile.skip_positions, MAX_SKIP_POSITIONS_TOTAL);
}

// Gets the preview profile number/ID.
// @return The preview profile ID.
uint8_t ProfileManager::getPreviewProfileNum(void) {
  return previewProfileIndex;
}

// Gets a reference to the preview profile.
// @return Reference to the preview profile.
PreviewProfile& ProfileManager::getPreviewProfile(void) {
  return previewProfile;
}
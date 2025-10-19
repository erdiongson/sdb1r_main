/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version

* Author: erdiongson
* Date Created: 2024.02.26
* Version 2.04: i. Changed the logic for the ReadPassEEPROM and WritePassEEPROM
*                  to read and write the characters of the password;
*/

#include "Profile.h"
#include <EEPROM.h>
#include "../gpu/Platform.h"
#include "../Utils.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../views/common/Keyboards.h"
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
    currentProfile.vibration_enabled = VIBRATION_LEVEL_DEFAULT;
    currentProfile.password_enabled = true;
    currentProfile.vibration_duration = VIBRATION_DURATION_DEFAULT;
    currentProfile.z_dip = Z_DIP_DEFAULT;
    currentProfile.staggered = false;
    currentProfile.skip_count = 0;
    for (int j = 0; j < MAX_SKIP_POSITIONS; j++) {
      currentProfile.skip_positions[j] = SkipPosition(0, 0);
    }

    writeProfileEEPROM(i);
  }

  writeCurIDEEPROM(0);  // reset current profile in eeprom to 0
  readProfileEEPROM(0);  // reload profile 0 into memory
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
  Logger::log("PROFILE SIZE: " + String(PROFILE_SIZE));

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
    pass[i++] = c;          // Store the character in pass and increment the index
  } while (c != '\0');  // Repeat until null terminator is encountered
}

void ProfileManager::writePassEEPROM(char* pass) {
  int addr = PASSWORD_EEPROM_ADDR;
  int i = 0;
  char c;
  do {
    c = pass[i++];          // Get the next character from the pass array and increment the index
    EEPROM.put(addr++, c);  // Write the character to EEPROM
  } while (c != '\0');  // Repeat until null terminator is encountered
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
  Logger::log("Starting Address = ", (float)address);

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
  EEPROM.put(address, currentProfile.vibration_enabled);
  address += sizeof(currentProfile.vibration_enabled);
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

  Logger::log("Ending Address = ", (float)address);
}

void ProfileManager::readProfileEEPROM(int index) {
  int address = (index * RESERVED_PROFILE_SIZE) + sizeof(uint8_t);
  Logger::log("Starting Address = ", (float)address);

  EEPROM.get(address, currentProfile.profile_name);
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
  EEPROM.get(address, currentProfile.vibration_enabled);
  address += sizeof(currentProfile.vibration_enabled);
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

  Logger::log("Ending Address = ", (float)address);
  checkProfile();
}

// Verifies the password by prompting the user for input.
// @param phost GPU context for displaying the keyboard.
// @return PasswordVerificationResult indicating success, incorrect, or cancelled.
PasswordVerificationResult ProfileManager::verifyPassword(Gpu_Hal_Context_t* phost) {
  char currentPassword[PASSWORD_MAX_LEN] = "";
  char inputPassword[PASSWORD_MAX_LEN] = "";

  readPassEEPROM(currentPassword);
  if (strcmp(currentPassword, "") == 0) strcpy_P(currentPassword, INITIAL_PASSWORD);

  KeyboardResult kbResult = getKeyboardValue(phost, inputPassword, "Enter Password", FALSE, PASSWORD_MAX_LEN, NULL);

  // Cancelled
  if (kbResult.action == ACTION_BACK || strcmp(inputPassword, "") == 0) {
    return PASSWORD_CANCELLED;
  }

  // Attempted
  bool passwordValid = (strcmp(currentPassword, inputPassword) == 0 || strcmp_P(inputPassword, SUPER_PASSWORD) == 0);
  if (!passwordValid) {
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
// @param profileNum The profile number to set as current.
void ProfileManager::setCurrentProfileNum(uint8_t profileNum) {
  if (profileNum < MAX_PROFILES) {
    currentProfileIndex = profileNum;
  }
}

// Gets skip data as char strings for UI display.
// @param outSkipCol Output buffer for column skip string (must be at least SKIP_STRING_LEN).
// @param outSkipRow Output buffer for row skip string (must be at least SKIP_STRING_LEN).
// @param outSkipSinglePos Output buffer for individual position skip string (must be at least SKIP_STRING_LEN).
void ProfileManager::getSkipStrings(char* outSkipCol, char* outSkipRow, char* outSkipSinglePos) {
  SkipUtils::convertToStrings(currentProfile.skip_positions, currentProfile.skip_count,
                              outSkipCol, outSkipRow, outSkipSinglePos);
}

// Sets skip data from char strings (from UI).
// @param skipCol Column skip string (format: "C1,C9,...").
// @param skipRow Row skip string (format: "R1,R9,...").
// @param skipSinglePos Individual position skip string (format: "C2R4,C3R4,...").
void ProfileManager::setSkipStrings(const char* skipCol, const char* skipRow, const char* skipSinglePos) {
  currentProfile.skip_count = SkipUtils::convertFromStrings(skipCol, skipRow, skipSinglePos,
                                                             currentProfile.skip_positions, MAX_SKIP_POSITIONS);
}
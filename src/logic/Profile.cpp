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

const int PROFILE_SIZE = sizeof(Profile);
static_assert(PROFILE_SIZE <= RESERVED_PROFILE_SIZE, "Profile size exceeds reserved space");
static_assert(EEPROM_SIZE >= MAX_PROFILES * RESERVED_PROFILE_SIZE, "EEPROM size is too small");

void ProfileManager::blankEEPROM(void) {
  for (int i = 0; i < EEPROM_SIZE; i++)
    EEPROM.put(i, 0);
}

void ProfileManager::preLoadEEPROM(void) {
  char buf[20];

  for (int i = 0; i < MAX_PROFILES; i++) {
    sprintf(buf, "profile %d", i + 1);
    strcpy(currentProfile.profile_name, buf);

    currentProfile.tube_no_x = TUBES_X_MAX;
    currentProfile.tube_no_y = TUBES_Y_MAX;
    currentProfile.pitch_x = PITCH_DEFAULT;
    currentProfile.pitch_y = PITCH_DEFAULT;
    currentProfile.tray_origin_x = ORIGIN_DEFAULT;
    currentProfile.tray_origin_y = ORIGIN_DEFAULT;
    currentProfile.cycles = CYCLES_DEFAULT;
    currentProfile.vibration_enabled = VIBRATION_LEVEL_DEFAULT;
    currentProfile.password_enabled = TRUE;
    currentProfile.vibration_duration = VIBRATION_DURATION_DEFAULT;
    currentProfile.z_dip = Z_DIP_DEFAULT;
    currentProfile.skip_col[0] = '\0';
    currentProfile.skip_row[0] = '\0';
    currentProfile.skip_single_pos[0] = '\0';
    currentProfile.staggered = false;

    writeProfileEEPROM(i);
  }

  writeCurIDEEPROM(0);  // reset current profile in eeprom to 0
}

void ProfileManager::checkProfile(void) {
  if (currentProfile.cycles > CYCLES_MAX) currentProfile.cycles = CYCLES_MAX;
  if (currentProfile.cycles < 0) currentProfile.cycles = 0;
  if (currentProfile.pitch_x > PITCH_X_MAX) currentProfile.pitch_x = PITCH_X_MAX;
  if (currentProfile.pitch_x < 0) currentProfile.pitch_x = 0;
  if (currentProfile.pitch_y > PITCH_Y_MAX) currentProfile.pitch_y = PITCH_Y_MAX;
  if (currentProfile.pitch_y < 0) currentProfile.pitch_y = 0;
  if (currentProfile.tray_origin_x > ORIGIN_X_MAX) currentProfile.tray_origin_x = ORIGIN_X_MAX;
  if (currentProfile.tray_origin_x < 0) currentProfile.tray_origin_x = 0;
  if (currentProfile.tray_origin_y > ORIGIN_Y_MAX) currentProfile.tray_origin_y = ORIGIN_Y_MAX;
  if (currentProfile.tray_origin_y < 0) currentProfile.tray_origin_y = 0;
  if (currentProfile.tube_no_x > TUBES_X_MAX) currentProfile.tube_no_x = TUBES_X_MAX;
  if (currentProfile.tube_no_x < 0) currentProfile.tube_no_x = 0;
  if (currentProfile.tube_no_y > TUBES_Y_MAX) currentProfile.tube_no_y = TUBES_Y_MAX;
  if (currentProfile.tube_no_y < 0) currentProfile.tube_no_y = 0;
  if (currentProfile.z_dip < 0) currentProfile.z_dip = 0;
  if (currentProfile.z_dip > Z_DIP_MAX) currentProfile.z_dip = Z_DIP_MAX;
}

uint8_t ProfileManager::loadProfile(void) {
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
  if (currentProfile.z_dip < 0) currentProfile.z_dip = 0;
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
  EEPROM.put(address, currentProfile.dispense_enabled);
  address += sizeof(currentProfile.dispense_enabled);
  EEPROM.put(address, currentProfile.z_dip);
  address += sizeof(currentProfile.z_dip);
  EEPROM.put(address, currentProfile.skip_col);
  address += sizeof(currentProfile.skip_col);
  EEPROM.put(address, currentProfile.skip_row);
  address += sizeof(currentProfile.skip_row);
  EEPROM.put(address, currentProfile.skip_single_pos);
  address += sizeof(currentProfile.skip_single_pos);
  EEPROM.put(address, currentProfile.staggered);
  address += sizeof(currentProfile.staggered);

  Logger::log("Ending Address = ", (float)address);
}

void ProfileManager::readProfileEEPROM(int index) {
  char buf[10];

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
  EEPROM.get(address, currentProfile.dispense_enabled);
  address += sizeof(currentProfile.dispense_enabled);
  EEPROM.get(address, currentProfile.z_dip);
  address += sizeof(currentProfile.z_dip);
  EEPROM.get(address, currentProfile.skip_col);
  address += sizeof(currentProfile.skip_col);
  EEPROM.get(address, currentProfile.skip_row);
  address += sizeof(currentProfile.skip_row);
  EEPROM.get(address, currentProfile.skip_single_pos);
  address += sizeof(currentProfile.skip_single_pos);
  EEPROM.get(address, currentProfile.staggered);
  address += sizeof(currentProfile.staggered);

  Logger::log("Ending Address = ", (float)address);
  checkProfile();
}

// Verifies the password by prompting the user for input.
// @param phost GPU context for displaying the keyboard.
// @return PasswordVerificationResult indicating success, incorrect, or cancelled.
PasswordVerificationResult ProfileManager::verifyPassword(Gpu_Hal_Context_t* phost) {
  char currentPassword[PROFILE_NAME_MAX_LEN] = "";
  char inputPassword[PROFILE_NAME_MAX_LEN] = "";

  readPassEEPROM(currentPassword);
  if (strcmp(currentPassword, "") == 0) strcpy(currentPassword, INITIAL_PASSWORD);

  getKeyboardValue(phost, inputPassword, "Enter Password", FALSE);

  // Cancelled
  if (strcmp(inputPassword, "") == 0) {
    return PASSWORD_CANCELLED;
  }

  // Attempted
  bool passwordValid = (strcmp(currentPassword, inputPassword) == 0 || strcmp(SUPER_PASSWORD, inputPassword) == 0);
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
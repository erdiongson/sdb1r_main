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
  for (int i = 0; i < EEPROM_SIZE; i++) EEPROM.put(i, 0);
}

void ProfileManager::preLoadEEPROM(void) {
  char buf[20];

  for (int i = 0; i < MAX_PROFILES; i++) {
    sprintf(buf, "profile %d", i + 1);
    strcpy(currentProfile.profile_name, buf);

    currentProfile.tube_no_x = MAX_BUTTONS_X;
    currentProfile.tube_no_y = MAX_BUTTONS_Y;
    currentProfile.pitch_x = 9;
    currentProfile.pitch_y = 9;
    currentProfile.tray_origin_x = 10;
    currentProfile.tray_origin_y = 10;
    currentProfile.cycles = 2;
    currentProfile.vibration_enabled = 0;
    currentProfile.password_enabled = TRUE;
    currentProfile.vibration_duration = 2;
    currentProfile.size_flag = 1;
    currentProfile.z_dip = 0.0;
    currentProfile.skip_col[0] = '\0';
    currentProfile.skip_row[0] = '\0';
    currentProfile.skip_single_pos[0] = '\0';
    currentProfile.staggered = false;

    writeProfileEEPROM(i);
  }

  writeCurIDEEPROM(0);  //reset current profile in eeprom to 0
}

void ProfileManager::checkProfile(void) {
  if (currentProfile.cycles > MAXCYCLE) currentProfile.cycles = MAXCYCLE;
  if (currentProfile.cycles < 0) currentProfile.cycles = 0;
  if (currentProfile.pitch_x > MAXPITCHX) currentProfile.pitch_x = MAXPITCHX;
  if (currentProfile.pitch_x < 0) currentProfile.pitch_x = 0;
  if (currentProfile.pitch_y > MAXPITCHY) currentProfile.pitch_y = MAXPITCHY;
  if (currentProfile.pitch_y < 0) currentProfile.pitch_y = 0;
  if (currentProfile.tray_origin_x > MAXORGX) currentProfile.tray_origin_x = MAXORGX;
  if (currentProfile.tray_origin_x < 0) currentProfile.tray_origin_x = 0;
  if (currentProfile.tray_origin_y > MAXORGY) currentProfile.tray_origin_y = MAXORGY;
  if (currentProfile.tray_origin_y < 0) currentProfile.tray_origin_y = 0;
  if (currentProfile.tube_no_x > MAX_TUBES_X) currentProfile.tube_no_x = MAX_TUBES_X;
  if (currentProfile.tube_no_x < 0) currentProfile.tube_no_x = 0;
  if (currentProfile.tube_no_y > MAX_TUBES_Y) currentProfile.tube_no_y = MAX_TUBES_Y;
  if (currentProfile.tube_no_y < 0) currentProfile.tube_no_y = 0;
  if (currentProfile.z_dip < 0) currentProfile.z_dip = 0;
  if (currentProfile.z_dip > MAX_ZDIP) currentProfile.z_dip = MAX_ZDIP;
}

uint8_t ProfileManager::loadProfile(void) {

  this->currentProfileIndex = readCurIDEEPROM();
  if (this->currentProfileIndex >= MAX_PROFILES) this->currentProfileIndex = 0;  // if corrcupt data from eeprom, set id=0

  readProfileEEPROM(this->currentProfileIndex);  //reread as it is not profile id 0
  if (currentProfile.cycles > MAXCYCLE) currentProfile.cycles = MAXCYCLE;
  if (currentProfile.pitch_x > MAXPITCHX) currentProfile.pitch_x = MAXPITCHX;
  if (currentProfile.pitch_y > MAXPITCHY) currentProfile.pitch_y = MAXPITCHY;
  if (currentProfile.tray_origin_x > MAXORGX) currentProfile.tray_origin_x = MAXORGX;
  if (currentProfile.tray_origin_y > MAXORGY) currentProfile.tray_origin_y = MAXORGY;
  if (currentProfile.tube_no_x > MAX_TUBES_X) currentProfile.tube_no_x = MAX_TUBES_X;
  if (currentProfile.tube_no_y > MAX_TUBES_Y) currentProfile.tube_no_y = MAX_TUBES_Y;
  if (currentProfile.z_dip < 0) currentProfile.z_dip = 0;
  if (currentProfile.z_dip > MAX_ZDIP) currentProfile.z_dip = MAX_ZDIP;

  return this->currentProfileIndex;
}

void ProfileManager::readPassEEPROM(char *pass) {
  int addr = 4000;
  char c;
  int i = 0;
  do {
    EEPROM.get(addr++, c);  // Read one character from EEPROM
    pass[i++] = c;          // Store the character in pass and increment the index
  } while (c != '\0');      // Repeat until null terminator is encountered
}

void ProfileManager::writePassEEPROM(char *pass) {
  int addr = 4000;
  int i = 0;
  char c;
  do {
    c = pass[i++];          // Get the next character from the pass array and increment the index
    EEPROM.put(addr++, c);  // Write the character to EEPROM
  } while (c != '\0');      // Repeat until null terminator is encountered
}

void ProfileManager::writeCurIDEEPROM(uint8_t curprofid) {
  EEPROM.put(0, curprofid);
}

uint8_t ProfileManager::readCurIDEEPROM(void) {
  uint8_t temp;

  EEPROM.get(0, temp);
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
  EEPROM.put(address, currentProfile.size_flag);
  address += sizeof(currentProfile.size_flag);
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
  EEPROM.get(address, currentProfile.size_flag);
  address += sizeof(currentProfile.size_flag);
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
PasswordVerificationResult ProfileManager::verifyPassword(Gpu_Hal_Context_t *phost) {
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
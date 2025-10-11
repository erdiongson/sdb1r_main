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

void ProfileManager::blankEEPROM(void) {
  for (int i = 0; i < 4096; i++) EEPROM.put(i, 0);
}

void ProfileManager::preLoadEEPROM(void) {
  char buf[20];

  for (int i = 0; i < MAX_PROFILES; i++) {
    sprintf(buf, "profile %d", i + 1);
    strcpy(CurProf.profile_name, buf);

    CurProf.tube_no_x = MAX_BUTTONS_X;
    CurProf.tube_no_y = MAX_BUTTONS_Y;
    CurProf.pitch_x = 9;
    CurProf.pitch_y = 9;
    CurProf.tray_origin_x = 10;
    CurProf.tray_origin_y = 10;
    CurProf.cycles = 2;
    CurProf.vibration_enabled = 0;
    CurProf.password_enabled = TRUE;
    CurProf.vibration_duration = 2;
    CurProf.size_flag = 1;
    CurProf.z_dip = 0.0;
    CurProf.skip_col[0] = '\0';
    CurProf.skip_row[0] = '\0';
    CurProf.skip_single_pos[0] = '\0';
    CurProf.staggered = false;

    writeProfileEEPROM(i);
  }

  writeCurIDEEPROM(0);  //reset current profile in eeprom to 0
}

void ProfileManager::checkProfile(void) {
  if (CurProf.cycles > MAXCYCLE) CurProf.cycles = MAXCYCLE;
  if (CurProf.cycles < 0) CurProf.cycles = 0;
  if (CurProf.pitch_x > MAXPITCHX) CurProf.pitch_x = MAXPITCHX;
  if (CurProf.pitch_x < 0) CurProf.pitch_x = 0;
  if (CurProf.pitch_y > MAXPITCHY) CurProf.pitch_y = MAXPITCHY;
  if (CurProf.pitch_y < 0) CurProf.pitch_y = 0;
  if (CurProf.tray_origin_x > MAXORGX) CurProf.tray_origin_x = MAXORGX;
  if (CurProf.tray_origin_x < 0) CurProf.tray_origin_x = 0;
  if (CurProf.tray_origin_y > MAXORGY) CurProf.tray_origin_y = MAXORGY;
  if (CurProf.tray_origin_y < 0) CurProf.tray_origin_y = 0;
  if (CurProf.tube_no_x > MAX_TUBES_X) CurProf.tube_no_x = MAX_TUBES_X;
  if (CurProf.tube_no_x < 0) CurProf.tube_no_x = 0;
  if (CurProf.tube_no_y > MAX_TUBES_Y) CurProf.tube_no_y = MAX_TUBES_Y;
  if (CurProf.tube_no_y < 0) CurProf.tube_no_y = 0;
  if (CurProf.z_dip < 0) CurProf.z_dip = 0;
  if (CurProf.z_dip > MAX_ZDIP) CurProf.z_dip = MAX_ZDIP;
}

uint8_t ProfileManager::loadProfile(void) {

  int currentProfileIndex = readCurIDEEPROM();
  if (currentProfileIndex >= MAX_PROFILES) currentProfileIndex = 0;  // if corrcupt data from eeprom, set id=0

  readProfileEEPROM(currentProfileIndex);  //reread as it is not profile id 0
  if (CurProf.cycles > MAXCYCLE) CurProf.cycles = MAXCYCLE;
  if (CurProf.pitch_x > MAXPITCHX) CurProf.pitch_x = MAXPITCHX;
  if (CurProf.pitch_y > MAXPITCHY) CurProf.pitch_y = MAXPITCHY;
  if (CurProf.tray_origin_x > MAXORGX) CurProf.tray_origin_x = MAXORGX;
  if (CurProf.tray_origin_y > MAXORGY) CurProf.tray_origin_y = MAXORGY;
  if (CurProf.tube_no_x > MAX_TUBES_X) CurProf.tube_no_x = MAX_TUBES_X;
  if (CurProf.tube_no_y > MAX_TUBES_Y) CurProf.tube_no_y = MAX_TUBES_Y;
  if (CurProf.z_dip < 0) CurProf.z_dip = 0;
  if (CurProf.z_dip > MAX_ZDIP) CurProf.z_dip = MAX_ZDIP;

  return currentProfileIndex;
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
  int address = (index * 300) + sizeof(uint8_t);
  Dprint("Starting Address = ", (float)address);

  EEPROM.put(address, CurProf.profile_name);
  address += sizeof(CurProf.profile_name);
  EEPROM.put(address, CurProf.tube_no_x);
  address += sizeof(CurProf.tube_no_x);
  EEPROM.put(address, CurProf.tube_no_y);
  address += sizeof(CurProf.tube_no_y);
  EEPROM.put(address, CurProf.pitch_x);
  address += sizeof(CurProf.pitch_x);
  EEPROM.put(address, CurProf.pitch_y);
  address += sizeof(CurProf.pitch_y);
  EEPROM.put(address, CurProf.tray_origin_x);
  address += sizeof(CurProf.tray_origin_x);
  EEPROM.put(address, CurProf.tray_origin_y);
  address += sizeof(CurProf.tray_origin_y);
  EEPROM.put(address, CurProf.cycles);
  address += sizeof(CurProf.cycles);
  EEPROM.put(address, CurProf.vibration_enabled);
  address += sizeof(CurProf.vibration_enabled);
  EEPROM.put(address, CurProf.password_enabled);
  address += sizeof(CurProf.password_enabled);
  EEPROM.put(address, CurProf.vibration_duration);
  address += sizeof(CurProf.vibration_duration);
  EEPROM.put(address, CurProf.dispense_enabled);
  address += sizeof(CurProf.dispense_enabled);
  EEPROM.put(address, CurProf.size_flag);
  address += sizeof(CurProf.size_flag);
  EEPROM.put(address, CurProf.z_dip);
  address += sizeof(CurProf.z_dip);
  EEPROM.put(address, CurProf.skip_col);
  address += sizeof(CurProf.skip_col);
  EEPROM.put(address, CurProf.skip_row);
  address += sizeof(CurProf.skip_row);
  EEPROM.put(address, CurProf.skip_single_pos);
  address += sizeof(CurProf.skip_single_pos);
  EEPROM.put(address, CurProf.staggered);
  address += sizeof(CurProf.staggered);

  Dprint("Ending Address = ", (float)address);
}

void ProfileManager::readProfileEEPROM(int index) {
  char buf[10];

  int address = (index * 300) + sizeof(uint8_t);
  Dprint("Starting Address = ", (float)address);

  EEPROM.get(address, CurProf.profile_name);
  address += sizeof(CurProf.profile_name);
  EEPROM.get(address, CurProf.tube_no_x);
  address += sizeof(CurProf.tube_no_x);
  EEPROM.get(address, CurProf.tube_no_y);
  address += sizeof(CurProf.tube_no_y);
  EEPROM.get(address, CurProf.pitch_x);
  address += sizeof(CurProf.pitch_x);
  EEPROM.get(address, CurProf.pitch_y);
  address += sizeof(CurProf.pitch_y);
  EEPROM.get(address, CurProf.tray_origin_x);
  address += sizeof(CurProf.tray_origin_x);
  EEPROM.get(address, CurProf.tray_origin_y);
  address += sizeof(CurProf.tray_origin_y);
  EEPROM.get(address, CurProf.cycles);
  address += sizeof(CurProf.cycles);
  EEPROM.get(address, CurProf.vibration_enabled);
  address += sizeof(CurProf.vibration_enabled);
  EEPROM.get(address, CurProf.password_enabled);
  address += sizeof(CurProf.password_enabled);
  EEPROM.get(address, CurProf.vibration_duration);
  address += sizeof(CurProf.vibration_duration);
  EEPROM.get(address, CurProf.dispense_enabled);
  address += sizeof(CurProf.dispense_enabled);
  EEPROM.get(address, CurProf.size_flag);
  address += sizeof(CurProf.size_flag);
  EEPROM.get(address, CurProf.z_dip);
  address += sizeof(CurProf.z_dip);
  EEPROM.get(address, CurProf.skip_col);
  address += sizeof(CurProf.skip_col);
  EEPROM.get(address, CurProf.skip_row);
  address += sizeof(CurProf.skip_row);
  EEPROM.get(address, CurProf.skip_single_pos);
  address += sizeof(CurProf.skip_single_pos);
  EEPROM.get(address, CurProf.staggered);
  address += sizeof(CurProf.staggered);

  Dprint("Ending Address = ", (float)address);
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
  Serial.println("Current password:" + String(currentPassword));

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
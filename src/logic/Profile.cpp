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

void BlankEEPROM(void) {
  for (int i = 0; i < 4096; i++) EEPROM.put(i, 0);
}

void PreLoadEEPROM(void) {
  char buf[20];

  for (int i = 0; i < MAX_PROFILES; i++) {
    sprintf(buf, "profile %d", i + 1);
    strcpy(CurProf.profileName, buf);

    CurProf.Tube_No_x = MAX_BUTTONS_X;
    CurProf.Tube_No_y = MAX_BUTTONS_Y;
    CurProf.pitch_x = 9;
    CurProf.pitch_y = 9;
    CurProf.trayOriginX = 10;
    CurProf.trayOriginY = 10;
    CurProf.Cycles = 2;
    CurProf.vibrationEnabled = 0;
    CurProf.passwordEnabled = TRUE;
    CurProf.vibrationDuration = 2;
    CurProf.sizeFlag = 1;
    CurProf.ZDip = 0.0;
    CurProf.skipCol[0] = '\0';
    CurProf.skipRow[0] = '\0';
    CurProf.skipSinglePos[0] = '\0';
    CurProf.staggered = false;

    WriteProfileEEPROM(i);
  }

  WriteCurIDEEPROM(0);  //reset current profile in eeprom to 0
}

void CheckProfile(void) {
  if (CurProf.Cycles > MAXCYCLE) CurProf.Cycles = MAXCYCLE;
  if (CurProf.Cycles < 0) CurProf.Cycles = 0;
  if (CurProf.pitch_x > MAXPITCHX) CurProf.pitch_x = MAXPITCHX;
  if (CurProf.pitch_x < 0) CurProf.pitch_x = 0;
  if (CurProf.pitch_y > MAXPITCHY) CurProf.pitch_y = MAXPITCHY;
  if (CurProf.pitch_y < 0) CurProf.pitch_y = 0;
  if (CurProf.trayOriginX > MAXORGX) CurProf.trayOriginX = MAXORGX;
  if (CurProf.trayOriginX < 0) CurProf.trayOriginX = 0;
  if (CurProf.trayOriginY > MAXORGY) CurProf.trayOriginY = MAXORGY;
  if (CurProf.trayOriginY < 0) CurProf.trayOriginY = 0;
  if (CurProf.Tube_No_x > MAX_TUBES_X) CurProf.Tube_No_x = MAX_TUBES_X;
  if (CurProf.Tube_No_x < 0) CurProf.Tube_No_x = 0;
  if (CurProf.Tube_No_y > MAX_TUBES_Y) CurProf.Tube_No_y = MAX_TUBES_Y;
  if (CurProf.Tube_No_y < 0) CurProf.Tube_No_y = 0;
  if (CurProf.ZDip < 0) CurProf.ZDip = 0;
  if (CurProf.ZDip > MAX_ZDIP) CurProf.ZDip = MAX_ZDIP;
}

uint8_t LoadProfile(void) {

  int currentProfileIndex = ReadCurIDEEPROM();
  if (currentProfileIndex >= MAX_PROFILES) currentProfileIndex = 0;  // if corrcupt data from eeprom, set id=0

  ReadProfileEEPROM(currentProfileIndex);  //reread as it is not profile id 0
  if (CurProf.Cycles > MAXCYCLE) CurProf.Cycles = MAXCYCLE;
  if (CurProf.pitch_x > MAXPITCHX) CurProf.pitch_x = MAXPITCHX;
  if (CurProf.pitch_y > MAXPITCHY) CurProf.pitch_y = MAXPITCHY;
  if (CurProf.trayOriginX > MAXORGX) CurProf.trayOriginX = MAXORGX;
  if (CurProf.trayOriginY > MAXORGY) CurProf.trayOriginY = MAXORGY;
  if (CurProf.Tube_No_x > MAX_TUBES_X) CurProf.Tube_No_x = MAX_TUBES_X;
  if (CurProf.Tube_No_y > MAX_TUBES_Y) CurProf.Tube_No_y = MAX_TUBES_Y;
  if (CurProf.ZDip < 0) CurProf.ZDip = 0;
  if (CurProf.ZDip > MAX_ZDIP) CurProf.ZDip = MAX_ZDIP;

  return currentProfileIndex;
}

void ReadPassEEPROM(char *pass) {
  int addr = 4000;
  char c;
  int i = 0;
  do {
    EEPROM.get(addr++, c);  // Read one character from EEPROM
    pass[i++] = c;          // Store the character in pass and increment the index
  } while (c != '\0');      // Repeat until null terminator is encountered
}

void WritePassEEPROM(char *pass) {
  int addr = 4000;
  int i = 0;
  char c;
  do {
    c = pass[i++];          // Get the next character from the pass array and increment the index
    EEPROM.put(addr++, c);  // Write the character to EEPROM
  } while (c != '\0');      // Repeat until null terminator is encountered
}

void WriteCurIDEEPROM(uint8_t curprofid) {
  EEPROM.put(0, curprofid);
}

uint8_t ReadCurIDEEPROM(void) {
  uint8_t temp;

  EEPROM.get(0, temp);
  if (temp > MAX_PROFILES) temp = 0;
  return temp;
}

void WriteProfileEEPROM(int index) {
  int address = (index * 300) + sizeof(uint8_t);
  Dprint("Starting Address = ", (float)address);

  EEPROM.put(address, CurProf.profileName);
  address += sizeof(CurProf.profileName);
  EEPROM.put(address, CurProf.Tube_No_x);
  address += sizeof(CurProf.Tube_No_x);
  EEPROM.put(address, CurProf.Tube_No_y);
  address += sizeof(CurProf.Tube_No_y);
  EEPROM.put(address, CurProf.pitch_x);
  address += sizeof(CurProf.pitch_x);
  EEPROM.put(address, CurProf.pitch_y);
  address += sizeof(CurProf.pitch_y);
  EEPROM.put(address, CurProf.trayOriginX);
  address += sizeof(CurProf.trayOriginX);
  EEPROM.put(address, CurProf.trayOriginY);
  address += sizeof(CurProf.trayOriginY);
  EEPROM.put(address, CurProf.Cycles);
  address += sizeof(CurProf.Cycles);
  EEPROM.put(address, CurProf.vibrationEnabled);
  address += sizeof(CurProf.vibrationEnabled);
  EEPROM.put(address, CurProf.passwordEnabled);
  address += sizeof(CurProf.passwordEnabled);
  EEPROM.put(address, CurProf.vibrationDuration);
  address += sizeof(CurProf.vibrationDuration);
  EEPROM.put(address, CurProf.dispenseEnabled);
  address += sizeof(CurProf.dispenseEnabled);
  EEPROM.put(address, CurProf.sizeFlag);
  address += sizeof(CurProf.sizeFlag);
  EEPROM.put(address, CurProf.ZDip);
  address += sizeof(CurProf.ZDip);
  EEPROM.put(address, CurProf.skipCol);
  address += sizeof(CurProf.skipCol);
  EEPROM.put(address, CurProf.skipRow);
  address += sizeof(CurProf.skipRow);
  EEPROM.put(address, CurProf.skipSinglePos);
  address += sizeof(CurProf.skipSinglePos);
  EEPROM.put(address, CurProf.staggered);
  address += sizeof(CurProf.staggered);

  Dprint("Ending Address = ", (float)address);
}

void ReadProfileEEPROM(int index) {
  char buf[10];

  int address = (index * 300) + sizeof(uint8_t);
  Dprint("Starting Address = ", (float)address);

  EEPROM.get(address, CurProf.profileName);
  address += sizeof(CurProf.profileName);
  EEPROM.get(address, CurProf.Tube_No_x);
  address += sizeof(CurProf.Tube_No_x);
  EEPROM.get(address, CurProf.Tube_No_y);
  address += sizeof(CurProf.Tube_No_y);
  EEPROM.get(address, CurProf.pitch_x);
  address += sizeof(CurProf.pitch_x);
  EEPROM.get(address, CurProf.pitch_y);
  address += sizeof(CurProf.pitch_y);
  EEPROM.get(address, CurProf.trayOriginX);
  address += sizeof(CurProf.trayOriginX);
  EEPROM.get(address, CurProf.trayOriginY);
  address += sizeof(CurProf.trayOriginY);
  EEPROM.get(address, CurProf.Cycles);
  address += sizeof(CurProf.Cycles);
  EEPROM.get(address, CurProf.vibrationEnabled);
  address += sizeof(CurProf.vibrationEnabled);
  EEPROM.get(address, CurProf.passwordEnabled);
  address += sizeof(CurProf.passwordEnabled);
  EEPROM.get(address, CurProf.vibrationDuration);
  address += sizeof(CurProf.vibrationDuration);
  EEPROM.get(address, CurProf.dispenseEnabled);
  address += sizeof(CurProf.dispenseEnabled);
  EEPROM.get(address, CurProf.sizeFlag);
  address += sizeof(CurProf.sizeFlag);
  EEPROM.get(address, CurProf.ZDip);
  address += sizeof(CurProf.ZDip);
  EEPROM.get(address, CurProf.skipCol);
  address += sizeof(CurProf.skipCol);
  EEPROM.get(address, CurProf.skipRow);
  address += sizeof(CurProf.skipRow);
  EEPROM.get(address, CurProf.skipSinglePos);
  address += sizeof(CurProf.skipSinglePos);
  EEPROM.get(address, CurProf.staggered);
  address += sizeof(CurProf.staggered);

  Dprint("Ending Address = ", (float)address);
  CheckProfile();
}

// Verifies the password by prompting the user for input.
// @param phost GPU context for displaying the keyboard.
// @return PasswordVerificationResult indicating success, incorrect, or cancelled.
PasswordVerificationResult verify_password(Gpu_Hal_Context_t *phost) {
  char currentPassword[PROFILE_NAME_MAX_LEN] = "";
  char inputPassword[PROFILE_NAME_MAX_LEN] = "";

  ReadPassEEPROM(currentPassword);
  if (strcmp(currentPassword, "") == 0) strcpy(currentPassword, INITIAL_PASSWORD);
  Serial.println("Current password:" + String(currentPassword));

  get_keyboard_value(phost, inputPassword, "Enter Password", FALSE);

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
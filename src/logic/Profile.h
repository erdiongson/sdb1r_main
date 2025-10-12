/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#pragma once

#ifndef _PROFILE_H_
#define _PROFILE_H_

#include <Arduino.h>
#include "../gpu/Platform.h"

#define EEPROM_SIZE 4096 // For an Arduino Mega 
#define MAX_PROFILES 10
#define RESERVED_PROFILE_SIZE 300
#define PROFILE_START_ADDR 0

#define PROFILE_NAME_MAX_LEN 30
#define PASSWORD_MAX_LEN 30
#define KEYPAD_MAX_LEN 6
#define ROW_COL_MAX_LEN 45

//Change MAX_BUTTONS_X and MAX_BUTTONS_Y depending on the use;
//for (L) 300x300 - MAX_BUTTONS_X 33; MAX_BUTTONS_Y 33
//for (S) 200x300 - MAX_BUTTONS_X 20; MAX_BUTTONS_Y 27
#define MAX_BUTTONS_X 42//try054 33// Columns - small = 20
#define MAX_BUTTONS_Y 33 // Rows   - small = 27
#define MAXORGX 999//v204 99.9
#define MAXORGY 999//v204 99.9

#define MINZDIP 0
#define MAXZDIP 999

#define MINNUMX 1
#define MINNUMY 1
//Change MAXNUMX and MAXNUMY depending on the use;
//for (L) 300x300 - MAXNUMX 33; MAXNUMY 33
//for (S) 200x300 - MAXNUMX 20; MAXNUMY 27
#define MAXNUMX 42 //try054 33
#define MAXNUMY 33


#define MINPITCHX 9 //v205
#define MINPITCHY 9 //v205
#define MAXPITCHX 999 //v204 99.9
#define MAXPITCHY 999 //v204 99.9
#define MAXCYCLE 99
#define MINCYCLE 1

typedef struct
{
    char profile_name[PROFILE_NAME_MAX_LEN];
    int8_t profile_id = 0;
    int8_t tube_no_x = 0; //represent the number of columns
    int8_t tube_no_y = 0; //represent the number of rows
    float pitch_x = 0.0;
    float pitch_y = 0.0;
    float tray_origin_x = 0.0;
    float tray_origin_y = 0.0;
    int16_t cycles = 1;
    int16_t current_cycle = 0; //New field for storing the current cycle
    int16_t vibration_enabled = 0;
    bool dispense_enabled = false;
    int16_t password_enabled = 0;
    int16_t vibration_duration = 2;
    int16_t size_flag = 1; //small = 0; large = 1;
    float z_dip = 0.0;
    char skip_col[ROW_COL_MAX_LEN];
    char skip_row[ROW_COL_MAX_LEN];
    char skip_single_pos[ROW_COL_MAX_LEN];
    bool staggered = false;
} Profile;

// Password verification result enum.
enum PasswordVerificationResult {
  PASSWORD_SUCCESS,
  PASSWORD_INCORRECT,
  PASSWORD_CANCELLED
};

// Manages profile operations including EEPROM storage and password verification.
class ProfileManager {
private:
  Profile currentProfile;
  uint8_t currentProfileIndex;

public:
  // Loads the current profile from EEPROM.
  // @return The index of the loaded profile.
  uint8_t loadProfile(void);

  // Preloads EEPROM with default profile data.
  void preLoadEEPROM(void);
  
  // Blanks the entire EEPROM.
  void blankEEPROM(void);

  // Writes the current profile ID to EEPROM.
  // @param curprofid The profile ID to write.
  void writeCurIDEEPROM(uint8_t curprofid);
  
  // Reads the current profile ID from EEPROM.
  // @return The current profile ID.
  uint8_t readCurIDEEPROM(void);
  
  // Writes the password to EEPROM.
  // @param pass The password string to write.
  void writePassEEPROM(char *pass);

  // Reads the password from EEPROM.
  // @param pass Buffer to store the password.
  void readPassEEPROM(char *pass);

  // Writes a profile to EEPROM at the specified index.
  // @param index The profile index.
  void writeProfileEEPROM(int index);
  
  // Reads a profile from EEPROM at the specified index.
  // @param index The profile index.
  void readProfileEEPROM(int index);

  // Verifies the password by prompting the user for input.
  // @param phost GPU context for displaying the keyboard.
  // @return PasswordVerificationResult indicating success, incorrect, or cancelled.
  PasswordVerificationResult verifyPassword(Gpu_Hal_Context_t *phost);

  // Checks and validates profile parameters.
  void checkProfile(void);

  // Gets a reference to the current profile.
  // @return Reference to the current profile.
  Profile& getCurrentProfile(void);

  // Sets the current profile.
  // @param profile The profile to set as current.
  void setCurrentProfile(const Profile& profile);

  // Gets the current profile number/ID.
  // @return The current profile ID.
  uint8_t getCurrentProfileNum(void);

  // Sets the current profile number/ID.
  // @param profileNum The profile number to set as current.
  void setCurrentProfileNum(uint8_t profileNum);
};

// Global profile_manager instance (defined in sdb1r_main.ino).
extern ProfileManager profile_manager;

#endif /*_PROFILE_H_*/

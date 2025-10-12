/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#pragma once

#include <Arduino.h>
#include "../gpu/Platform.h"
#include "../../Config.h"

typedef struct {
  char profile_name[PROFILE_NAME_MAX_LEN];
  int8_t tube_no_x = 0;  // represent the number of columns
  int8_t tube_no_y = 0;  // represent the number of rows
  float pitch_x = 0.0;
  float pitch_y = 0.0;
  float tray_origin_x = 0.0;
  float tray_origin_y = 0.0;
  int16_t cycles = 1;
  int16_t vibration_enabled = 0;
  bool dispense_enabled = false;
  int16_t password_enabled = 0;
  int16_t vibration_duration = 2;
  float z_dip = 0.0;
  char skip_col[ROW_COL_MAX_LEN];
  char skip_row[ROW_COL_MAX_LEN];
  char skip_single_pos[ROW_COL_MAX_LEN];
  bool staggered = false;
} Profile;

// Password verification result enum.
enum PasswordVerificationResult { PASSWORD_SUCCESS, PASSWORD_INCORRECT, PASSWORD_CANCELLED };

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
  void writePassEEPROM(char* pass);

  // Reads the password from EEPROM.
  // @param pass Buffer to store the password.
  void readPassEEPROM(char* pass);

  // Writes a profile to EEPROM at the specified index.
  // @param index The profile index.
  void writeProfileEEPROM(int index);

  // Reads a profile from EEPROM at the specified index.
  // @param index The profile index.
  void readProfileEEPROM(int index);

  // Verifies the password by prompting the user for input.
  // @param phost GPU context for displaying the keyboard.
  // @return PasswordVerificationResult indicating success, incorrect, or cancelled.
  PasswordVerificationResult verifyPassword(Gpu_Hal_Context_t* phost);

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

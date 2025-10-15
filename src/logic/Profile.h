/* Author : XentiQ
Date created - 2022.12.14 - XentiQ version
*/

#pragma once

#include <Arduino.h>
#include "../gpu/Platform.h"
#include "../../Config.h"

// SkipPosition struct for compact storage in EEPROM.
struct SkipPosition {
  uint8_t x;  // Column number (0-255)
  uint8_t y;  // Row number (0-255)

  SkipPosition() : x(0), y(0) {}
  SkipPosition(uint8_t x, uint8_t y) : x(x), y(y) {}
};

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
  int16_t password_enabled = 0;
  int16_t vibration_duration = 2;
  float z_dip = 0.0;
  SkipPosition skip_positions[MAX_SKIP_POSITIONS];  // Compact storage for EEPROM
  uint8_t skip_count = 0;                           // Number of active skip positions
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

  // Gets skip data as char strings for UI display.
  // @param outSkipCol Output buffer for column skip string (must be at least SKIP_STRING_LEN).
  // @param outSkipRow Output buffer for row skip string (must be at least SKIP_STRING_LEN).
  // @param outSkipSinglePos Output buffer for individual position skip string (must be at least SKIP_STRING_LEN).
  void getSkipStrings(char* outSkipCol, char* outSkipRow, char* outSkipSinglePos);

  // Sets skip data from char strings (from UI).
  // @param skipCol Column skip string (format: "C1,C9,...").
  // @param skipRow Row skip string (format: "R1,R9,...").
  // @param skipSinglePos Individual position skip string (format: "C2R4,C3R4,...").
  void setSkipStrings(const char* skipCol, const char* skipRow, const char* skipSinglePos);
};

// Global profile_manager instance (defined in sdb1r_main.ino).
extern ProfileManager profile_manager;

#include "SettingsMode.h"
#include "../../ui/App_Common.h"
#include "../../../Config.h"

SettingsMode::SettingsMode(DispenserHead& head, Gpu_Hal_Context_t *host)
  : BaseMode(head, host), selectedSetting(0), settingsModified(false), currentProfile(nullptr), specialMode(false) {}

void SettingsMode::handleValidPassword() {
  // Check for special mode actions
  if (specialMode) {
    if (digitalRead(Limit_S_y_MAX) != 0) {
      BlankEEPROM();
    } else {
      specialMode = false;
    }
  }
  
  // Display configuration screen
  DisplayConfig(phost);
  WaitKeyRelease();
  delay(100);
  
  // Enter configuration settings menu
  Config_Settings(phost);
  Serial.println("Config_Settings ended!");
  
  exitSettingsMode();
}

void SettingsMode::exitSettingsMode() {
  // Set flag to exit this mode
  Serial.println("Exiting Settings mode");
  settingsModified = true;
}

void SettingsMode::on_start(Profile& profile) {
  Serial.println("MODE: Settings mode");
  
  // Store reference to the current profile
  currentProfile = &profile;
  
  // Check if special mode is enabled
  specialMode = (digitalRead(Limit_S_y_MAX) == 0);
  
  // Reset password buffer
  Password[2][0] = 0;
  
  WaitKeyRelease();
  
  // Handle password protection if enabled
  if (currentProfile->passwordEnabled) {
    Keyboard(phost, Password[2], "Enter Password", FALSE);
    
    if (strcmp(Password[1], Password[2]) == 0 || strcmp(Password[0], Password[2]) == 0) {
      handleValidPassword();
    } else {
      // Show wrong password message
      DisplayKeyboard(phost, 0, "Wrong Password", " ", FALSE, FALSE, FALSE);
      delay(2000);
      Serial.println("Wrong Password!!! exiting!");
      exitSettingsMode();
    }
  } else {
    handleValidPassword();
  }
  Serial.println("ON START END");
}

void SettingsMode::on_interaction(const Interaction& interaction) {
  // Most interactions are handled within Config_Settings
  if (interaction.key_pressed > 0) {
    switch (interaction.key_pressed) {
      // Tag 6 is used for the Home button in DisplayConfig
      case 6: // Home button tag
        settingsModified = true;
        break;
        
      // Handle vibration level changes (tag 28 in Platform.h is VIBLVL)
      case VIBLVL:
        vibration_on();
        break;
        
      // Handle vibration duration changes (tag 30 in Platform.h is VIBDURATION)
      case VIBDURATION:
        vibration_time();
        break;
        
      default:
        // Other interactions are handled by Config_Settings
        break;
    }
  }
}

int SettingsMode::on_step() {
  Serial.println("SettingsMode::on_step " + String(settingsModified));
  return settingsModified ? MODE_COMPLETE : MODE_CONTINUE;
}

int SettingsMode::get_mode_type() const {
  return MODE_TYPE_SETTINGS;
}

/**********************************************************************************************************
* @brief vibration_on()
* @details Command for changing the vibration levels from U0-U4.
**********************************************************************************************************/
void SettingsMode::vibration_on() {
  bool success = false;

  // Cycle through vibration levels
  if (currentProfile->vibrationEnabled == 0) {
    currentProfile->vibrationEnabled = 1;
    success = dispenserHead.set_vibration_level(1);  // VIBMODE_U1
    if (!success) Serial.println("U1 Error");
  } else if (currentProfile->vibrationEnabled == 1) {
    currentProfile->vibrationEnabled = 2;
    success = dispenserHead.set_vibration_level(2);  // VIBMODE_U2
    if (!success) Serial.println("U2 Error");
  } else if (currentProfile->vibrationEnabled == 2) {
    currentProfile->vibrationEnabled = 3;
    success = dispenserHead.set_vibration_level(3);  // VIBMODE_U3
    if (!success) Serial.println("U3 Error");
  } else if (currentProfile->vibrationEnabled == 3) {
    currentProfile->vibrationEnabled = 4;
    success = dispenserHead.set_vibration_level(4);  // VIBMODE_U4
    if (!success) Serial.println("U4 Error");
  } else {
    currentProfile->vibrationEnabled = 0;
    success = dispenserHead.set_vibration_level(0);  // VIBMODE_U0
    if (!success) Serial.println("U0 Error");
  }
}

/**********************************************************************************************************
* @brief vibration_time()
* @details Command for changing the vibration duration/time from 1 to 5 seconds.
**********************************************************************************************************/
void SettingsMode::vibration_time() {
  bool success = false;

  // Cycle through vibration durations
  if (currentProfile->vibrationDuration == 2) {
    currentProfile->vibrationDuration = 3;
    success = dispenserHead.set_vibration_time(3);  // VIBDUR_3
  } else if (currentProfile->vibrationDuration == 3) {
    currentProfile->vibrationDuration = 4;
    success = dispenserHead.set_vibration_time(4);  // VIBDUR_4
  } else if (currentProfile->vibrationDuration == 4) {
    currentProfile->vibrationDuration = 5;
    success = dispenserHead.set_vibration_time(5);  // VIBDUR_5
  } else if (currentProfile->vibrationDuration == 5) {
    currentProfile->vibrationDuration = 1;
    success = dispenserHead.set_vibration_time(1);  // VIBDUR_1
  } else {
    currentProfile->vibrationDuration = 2;
    success = dispenserHead.set_vibration_time(2);  // VIBDUR_2
  }

  if (!success) {
    Serial.println("Vib Time Error");
  }
}

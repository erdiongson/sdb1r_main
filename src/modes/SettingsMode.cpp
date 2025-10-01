#include "SettingsMode.h"
#include "../views/Config_Screen.h"
#include "../../Config.h"

SettingsMode::SettingsMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback)
  : BaseMode(head, host, controller, callback), selectedSetting(0), settingsModified(false), currentProfile(nullptr), specialMode(false) {}

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
  Config_Screen(phost);
  WaitKeyRelease();
  delay(100);
  
  // Enter configuration settings menu
  // Config_Settings(phost);
  
  exitSettingsMode();
}

void SettingsMode::exitSettingsMode() {
  // Set flag to exit this mode
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
      exitSettingsMode();
    }
  } else {
    handleValidPassword();
  }
}

void SettingsMode::on_interaction(const Interaction& interaction) {
  // Most interactions are handled within Config_Settings
  if (interaction.key_pressed > 0) {
    switch (interaction.key_pressed) {
      // Tag 6 is used for the Home button in Config_Screen
      case 6: // Home button tag
        settingsModified = true;
        break;
        
      // Handle vibration level changes (tag 28 in Platform.h is VIBLVL)
      case VIBLVL:
        Serial.println("Incrementing vibration level");
        increment_vibration_level();
        break;
        
      // Handle vibration duration changes (tag 30 in Platform.h is VIBDURATION)
      case VIBDURATION:
        Serial.println("Incrementing vibration duration");
        increment_vibration_time();
        break;
        
      default:
        // Other interactions are handled by Config_Settings
        break;
    }
  }
}

int SettingsMode::on_step() {
  return settingsModified ? MODE_COMPLETE : MODE_CONTINUE;
}

int SettingsMode::get_mode_type() const {
  return MODE_TYPE_SETTINGS;
}

void SettingsMode::increment_vibration_level() {
  int next_level = currentProfile->vibrationEnabled + 1;
  if (next_level > 4) next_level = 0;

  currentProfile->vibrationEnabled = next_level;
  dispenserHead.set_vibration_level(next_level);
}

void SettingsMode::increment_vibration_time() {
  int next_duration = currentProfile->vibrationDuration + 1;
  if (next_duration > 5) next_duration = 1;

  currentProfile->vibrationDuration = next_duration;
  dispenserHead.set_vibration_time(next_duration);
}

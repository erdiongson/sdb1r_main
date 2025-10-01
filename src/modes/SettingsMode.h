#pragma once

#include "BaseMode.h"
#include "../gpu/Platform.h"

/**
 * SettingsMode - Responsible for managing system settings
 * 
 * This mode allows users to:
 * - View and modify system configuration
 * - Adjust calibration parameters
 * - Configure communication settings
 */
class SettingsMode : public BaseMode {
public:
  /**
   * Constructor
   * 
   * @param head Reference to the dispenser head to control
   * @param host Pointer to GPU HAL context
   */
  SettingsMode(DispenserHead& head, Gpu_Hal_Context_t *host);

  /**
   * Called when the mode is started
   * 
   * @param profile The current profile settings
   */
  void on_start(Profile& profile) override;

  /**
   * Called when an interaction occurs
   * 
   * @param interaction The interaction that occurred
   */
  void on_interaction(const Interaction& interaction) override;

  /**
   * Called on each step of the mode
   * 
   * @return MODE_COMPLETE when settings operations are finished, MODE_CONTINUE otherwise
   */
  int on_step() override;
  
  /**
   * Returns the type of this mode
   * 
   * @return MODE_TYPE_SETTINGS
   */
  int get_mode_type() const override;

private:
  // Current profile reference
  Profile* currentProfile;
  
  // Current selected setting index
  int selectedSetting;
  
  // Flag to track if settings have been modified
  bool settingsModified;

  // Flag to track if special mode is enabled
  bool specialMode;
  
  // Helper method to handle valid password entry
  void handleValidPassword();
  
  // Helper method to exit settings mode
  void exitSettingsMode();
  
  /**
   * @brief Change vibration level
   * @details Cycles through vibration levels from U0-U4
   */
  void vibration_on();
  
  /**
   * @brief Change vibration duration
   * @details Cycles through vibration durations from 1-5 seconds
   */
  void vibration_time();
};

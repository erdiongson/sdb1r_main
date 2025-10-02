#pragma once

#include "BaseMode.h"
#include "../gpu/Platform.h"

// ConfigMode - Responsible for managing system settings.
class ConfigMode : public BaseMode {
public:
  // Constructor.
  // @param head Reference to the dispenser head to control.
  // @param host Pointer to GPU HAL context.
  // @param controller Pointer to the mode controller for callbacks.
  // @param callback Callback function for mode completion.
  ConfigMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback);

  // Called when the mode is started.
  // @param profile The current profile settings.
  void on_start(Profile& profile) override;

  // Called when an interaction occurs.
  // @param interaction The interaction that occurred.
  void on_interaction(const Interaction& interaction) override;

  // Called on each step of the mode.
  // @return MODE_COMPLETE when settings operations are finished, MODE_CONTINUE otherwise.
  int on_step() override;
  
  // Returns the type of this mode.
  // @return MODE_TYPE_CONFIG.
  int get_mode_type() const override;

private:
  // Current profile reference
  Profile* currentProfile;
  
  // Flag to track if special mode is enabled
  bool specialMode;
  
  // Cycles through vibration levels from U0-U4.
  void increment_vibration_level();
  
  // Cycles through vibration durations from 1-5 seconds.
  void increment_vibration_time();
};

#pragma once

#include "BaseMode.h"

// DispenseTestMode - Responsible for testing dispenser functionality.
class DispenseTestMode : public BaseMode {
private:
  bool back = false;
  uint8_t current_vibration_level = 2;  // Default to U2 (medium)
  uint8_t current_vibration_time = 2;   // Default to 2 seconds

public:
  // Constructor.
  // @param head Reference to the dispenser head to control.
  // @param host Pointer to GPU HAL context.
  // @param controller Pointer to the mode controller for callbacks.
  // @param callback Callback function for mode completion.
  DispenseTestMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback);

  // Called when the mode is started.
  // @param profile The current profile settings.
  void on_start(Profile& profile) override;

  // Called when an interaction occurs.
  // @param interaction The interaction that occurred.
  void on_interaction(const Interaction& interaction) override;

  // Called on each step of the mode.
  // @return ModeStepResult with stepper and dispenser state.
  ModeStepResult on_step() override;
  
  // Returns the type of this mode.
  // @return MODE_TYPE_DISPENSE_TEST.
  int get_mode_type() const override;
  
  // Get the current vibration level setting.
  // @return Current vibration level (0-4).
  uint8_t get_current_vibration_level() const;
  
  // Get the current vibration time setting.
  // @return Current vibration time in seconds (1-5).
  uint8_t get_current_vibration_time() const;
};

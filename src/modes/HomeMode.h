#pragma once

#include "BaseMode.h"

/**
 * HomeMode - Responsible for homing the dispenser head
 * 
 * This mode moves the dispenser head to its home position:
 * - X axis to maximum position
 * - Y axis to minimum position
 * - Z axis to minimum position
 */
class HomeMode : public BaseMode {
public:
  /**
   * Constructor
   * 
   * @param head Reference to the dispenser head to control
   * @param host Pointer to GPU HAL context
   * @param controller Pointer to the mode controller for callbacks
   * @param callback Callback function for mode completion
   */
  HomeMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback);

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
   * @return MODE_COMPLETE when homing is finished, MODE_CONTINUE otherwise
   */
  int on_step() override;
  
  /**
   * Returns the type of this mode
   * 
   * @return MODE_TYPE_HOME
   */
  int get_mode_type() const override;
};

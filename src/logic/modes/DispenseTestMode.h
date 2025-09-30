#pragma once

#include "BaseMode.h"

/**
 * DispenseTestMode - Responsible for testing dispenser functionality
 * 
 * This mode allows testing of the dispenser with different settings:
 * - Vibration level (U0-U4)
 * - Vibration time (1-5 seconds)
 * - Manual dispense triggering
 */
class DispenseTestMode : public BaseMode {
private:
  bool back = false;
  uint8_t current_vibration_level = 2;  // Default to U2 (medium)
  uint8_t current_vibration_time = 2;   // Default to 2 seconds

public:
  /**
   * Constructor
   * 
   * @param head Reference to the dispenser head to control
   */
  DispenseTestMode(DispenserHead& head);

  /**
   * Called when the mode is started
   * 
   * @param profile The current profile settings
   */
  void on_start(Profile& profile) override;

  /**
   * Called when a button is pressed
   * 
   * @param button The button that was pressed
   */
  void on_button_pressed(int button) override;

  /**
   * Called on each step of the mode
   * 
   * @return MODE_COMPLETE when back button is pressed, MODE_CONTINUE otherwise
   */
  int on_step() override;
  
  /**
   * Returns the type of this mode
   * 
   * @return MODE_TYPE_DISPENSE_TEST
   */
  int get_mode_type() const override;
  
  /**
   * Get the current vibration level setting
   * 
   * @return Current vibration level (0-4)
   */
  uint8_t get_current_vibration_level() const;
  
  /**
   * Get the current vibration time setting
   * 
   * @return Current vibration time in seconds (1-5)
   */
  uint8_t get_current_vibration_time() const;
};

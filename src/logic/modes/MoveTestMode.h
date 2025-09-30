#pragma once

#include "BaseMode.h"

/**
 * MoveTestMode - Responsible for testing manual movement of the dispenser head
 * 
 * This mode allows manual control of the dispenser head movement:
 * - X axis movement using left/right buttons
 * - Y axis movement using up/down buttons
 * - Z axis movement using z-up/z-down buttons
 */
class MoveTestMode : public BaseMode {
private:
  bool back = false;

public:
  /**
   * Constructor
   * 
   * @param head Reference to the dispenser head to control
   */
  MoveTestMode(DispenserHead& head);

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
   * @return MODE_TYPE_MOVE_TEST
   */
  int get_mode_type() const override;
};

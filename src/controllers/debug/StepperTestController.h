#pragma once

#include "../../../Config.h"
#include "../BaseController.h"
#include "../../views/debug/StepperTestScreen.h"

class StepperTestController : public BaseController {
 public:
  StepperTestController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;

 private:
  // Updates the screen with current state.
  void updateScreen();

  // Loads the max speed and acceleration from the selected axis.
  void loadAxisParameters();

  // Gets a reference to the currently selected axis.
  Axis& getSelectedAxis();

  StepperTestAxis selected_axis = STEPPER_AXIS_X;  // Currently selected axis
  float move_amount_cm = DEFAULT_XY_DISTANCE_CM;   // Movement amount in cm
  float current_max_speed = 0;                     // Current max speed for selected axis
  float current_max_acceleration = 0;              // Current max acceleration for selected axis
  float original_max_speed = 0;                    // Original max speed for reset
  float original_max_acceleration = 0;             // Original max acceleration for reset
};

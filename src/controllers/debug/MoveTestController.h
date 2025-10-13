#pragma once

#include "../../../Config.h"
#include "../BaseController.h"

// Bounce stage enumeration
enum BounceStage {
  BOUNCE_STAGE_GO,
  BOUNCE_STAGE_RETURN
};

// Axis enumeration
enum BounceAxis {
  BOUNCE_AXIS_X,
  BOUNCE_AXIS_Y,
  BOUNCE_AXIS_Z
};

// Bounce state structure
struct BounceState {
  int current_count;
  int total_count;
  BounceStage current_stage;
  BounceAxis axis;
  int go_amount;
};

class MoveTestController : public BaseController {
 public:
  MoveTestController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;

 private:
  // Updates the screen with current state.
  void updateScreen();

  float xy_distance_cm = DEFAULT_XY_DISTANCE_CM;  // XY movement distance in cm
  float z_distance_cm = DEFAULT_Z_DISTANCE_CM;    // Z movement distance in cm
  int bounce_count = DEFAULT_BOUNCE_COUNT;        // Number of times to bounce back
  BounceState bounce_state = {0, 0, BOUNCE_STAGE_GO, BOUNCE_AXIS_X, 0};  // Bounce state tracker
};

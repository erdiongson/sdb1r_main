#pragma once

#include "../../../Config.h"
#include "../BaseController.h"

class MoveTestController : public BaseController {
 public:
  MoveTestController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;

 private:
  float xy_distance_cm = DEFAULT_XY_DISTANCE_CM;  // XY movement distance in cm
  float z_distance_cm = DEFAULT_Z_DISTANCE_CM;    // Z movement distance in cm
  int bounce_count = DEFAULT_BOUNCE_COUNT;        // Number of times to bounce back
};

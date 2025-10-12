#pragma once

#include "../BaseController.h"

class MoveTestController : public BaseController {
 public:
  MoveTestController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;

 private:
  float xy_distance_cm = 5.0;  // XY movement distance in cm
  float z_distance_cm = 3.0;   // Z movement distance in cm
  int bounce_count = 0;        // Number of times to bounce back
};

#pragma once

#include "BaseController.h"

class HomingController : public BaseController {
 private:
  bool running = true;
  unsigned long timeout_at = 0;

 public:
  HomingController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

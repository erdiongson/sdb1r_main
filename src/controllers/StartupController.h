#pragma once

#include "BaseController.h"

class StartupController : public BaseController {
 private:
  enum {
    STAGE_HANDSHAKE,
    STAGE_ERROR,
  };
  int stage = STAGE_HANDSHAKE;
  DispenserHead& dispenserHead;

 public:
  StartupController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

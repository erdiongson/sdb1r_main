#pragma once

#include "BaseController.h"
#include "../views/LogoScreen.h"

class StartupController : public BaseController {
 private:
  enum {
    STAGE_HANDSHAKE,
    STAGE_CLEAR,
    STAGE_HOME,
  };
  int stage = STAGE_HANDSHAKE;
  bool running = true;
  DispenserHead& dispenserHead;
  LogoScreenParams logo_params = {0, ""};

 public:
  StartupController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

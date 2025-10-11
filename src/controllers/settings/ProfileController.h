#pragma once

#include "../BaseController.h"

class ProfileController : public BaseController {
private:
  int8_t selectedProfileNum;

public:
  ProfileController(ControllerParams params);

  void onStart(Profile& profile) override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

#pragma once

#include "BaseController.h"

class ProfileController : public BaseController {
private:
  int8_t selectedProfileNum;

public:
  ProfileController(ControllerParams params);

  void on_start(Profile& profile) override;
  void on_interaction(const Interaction& interaction) override;
  ControllerStepResult on_step() override;

  int get_mode_type() const override;
};

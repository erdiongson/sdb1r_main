#pragma once

#include "../BaseController.h"

class ProfileController : public BaseController {
 private:
  int8_t selected_profile_num;

 public:
  ProfileController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

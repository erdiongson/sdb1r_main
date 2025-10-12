#pragma once

#include "../BaseController.h"

class DispenseTestController : public BaseController {
private:
  enum State {
    READY,
    WAITING_FOR_RESPONSE,
    RECEIVED_RESPONSE,
  };

  State state;
public:
  DispenseTestController(ControllerParams params);
  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};
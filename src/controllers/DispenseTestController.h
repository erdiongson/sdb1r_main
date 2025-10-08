#include "BaseController.h"

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
  void on_start(Profile& profile) override;
  void on_interaction(const Interaction& interaction) override;
  ControllerStepResult on_step() override;

  int get_mode_type() const override;
};
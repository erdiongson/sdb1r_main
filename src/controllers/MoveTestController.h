#include "BaseController.h"

class MoveTestController : public BaseController {
public:
  MoveTestController(ControllerParams params);

  void on_start(Profile& profile) override;
  void on_interaction(const Interaction& interaction) override;
  ControllerStepResult on_step() override;

  int get_mode_type() const override;
};

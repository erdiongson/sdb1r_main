#include "BaseController.h"

class HomingController : public BaseController {
private:
  enum {
    STAGE_CLEAR,
    STAGE_HOME,
    STAGE_ERROR,
  };
  int stage = STAGE_CLEAR;
public:
  HomingController(ControllerParams params);

  void on_start(Profile& profile) override;
  void on_interaction(const Interaction& interaction) override;
  ControllerStepResult on_step() override;

  int get_mode_type() const override;
};

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

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

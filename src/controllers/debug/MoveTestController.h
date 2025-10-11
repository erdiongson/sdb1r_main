#include "../BaseController.h"

class MoveTestController : public BaseController {
public:
  MoveTestController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

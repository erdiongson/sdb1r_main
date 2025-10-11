#include "../BaseController.h"

class DebugController : public BaseController {
public:
  DebugController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

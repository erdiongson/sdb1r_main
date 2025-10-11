#include "BaseController.h"

class ReadyController : public BaseController {
public:
  ReadyController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

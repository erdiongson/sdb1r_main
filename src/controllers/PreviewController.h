#include "BaseController.h"
#include "../logic/TrayPositionHandler.h"

class PreviewController : public BaseController {
private:
  Profile* currentProfile;
  bool simulating;
  unsigned long lastSimulationTime;
  TrayHandler::TrayPositionHandler simulationHandler;
  int simulateCol;
  int simulateRow;
  char previewInfoText[50];
  
  void start_simulation();
  void end_simulation();
  void step_simulation();

public:
  PreviewController(ControllerParams params);

  void on_start(Profile& profile) override;
  void on_interaction(const Interaction& interaction) override;
  ControllerStepResult on_step() override;

  int get_mode_type() const override;
};

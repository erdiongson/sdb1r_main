#include "../BaseController.h"
#include "../../logic/TrayPositionHandler.h"

class PreviewController : public BaseController {
private:
  Profile* currentProfile;
  bool simulating;
  unsigned long lastSimulationTime;
  TrayHandler::TrayPositionHandler simulationHandler;
  int simulateCol;
  int simulateRow;
  char previewInfoText[50];
  
  void startSimulation();
  void endSimulation();
  void stepSimulation();

public:
  PreviewController(ControllerParams params);

  void onStart(Profile& profile) override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

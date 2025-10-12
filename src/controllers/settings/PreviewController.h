#pragma once

#include "../BaseController.h"
#include "../../logic/TrayPositionHandler.h"

class PreviewController : public BaseController {
private:
  Profile* current_profile;
  bool simulating;
  unsigned long last_simulation_time;
  TrayHandler::TrayPositionHandler simulation_handler;
  int simulate_col;
  int simulate_row;
  char preview_info_text[50];
  
  void startSimulation();
  void endSimulation();
  void stepSimulation();

public:
  PreviewController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

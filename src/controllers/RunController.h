#pragma once

#include "BaseController.h"
#include "../logic/TrayPositionHandler.h"
#include "../views/MainScreen.h"
#include "../../Config.h"

class RunController : public BaseController {
 private:
  enum PauseState {
    NOT_PAUSED,
    PAUSING,
    PAUSED
  };

  PauseState pause_state = NOT_PAUSED;

  enum Stage {
    STAGE_IDLE,
    STAGE_SET_VIB_LEVEL,
    STAGE_SET_VIB_DURATION,
    STAGE_ZERO,
    STAGE_START_PRIME,
    STAGE_WAIT_PRIME,
    STAGE_LOWER_HEAD,
    STAGE_START_DISPENSE,
    STAGE_WAIT_DISPENSE,
    STAGE_RAISE_HEAD,
    STAGE_MOVE,
  };

  int stage = STAGE_IDLE;
  Profile profile;
  TrayHandler::TrayPositionHandler trayHandler;
  int32_t target_x = 0;
  int32_t target_y = 0;
  int cycle = 0;
  unsigned long timeout_at = 0;

  void processStageLogic(DispenserProcessResult& dispenser_process_result);
  void startStage(Stage new_stage);
  void requestPause();
  void pause();
  void resume();
  void stop();
  void start();
  
  // Handles limit switch error by logging, displaying error dialog, and pausing.
  // @param result The axis movement result code to check.
  // @return True if a limit switch error occurred, false otherwise.
  bool handleLimitSwitchError(int result);
  
  // Sets target_x and target_y to the first position in the tray.
  void setFirstTarget();
  
  // Prepares the current run status for display.
  // @return RunStatus struct with current position and progress information.
  RunStatus getRunStatus();

 public:
  RunController(ControllerParams params);

  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};

#include "HomingController.h"
#include "../views/MainScreen.h"

HomingController::HomingController(ControllerParams params) : BaseController(params) {}

void HomingController::onStart() {
  Logger::log(F("HomingController::on_start"));

  // Start the homing process
  dispenserHead.x().moveToMin();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMax();
}

void HomingController::onInteraction(const Interaction& interaction) {}

ControllerStepResult HomingController::onStep() {
  DispenserProcessResult result = dispenserHead.process();

  // Handle possible errors
  if (result.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    Profile& profile = profile_manager.getCurrentProfile();
    drawHomingScreen({ profile, { 0, 0, 0, 0 }, DIALOG_ERROR_LIMIT_SWITCH_HOMING });
    stage = STAGE_ERROR;
    return ControllerStepResult(false);
  }

  if (stage == STAGE_HOME && result.steppers == AXIS_STATE_COMPLETE) {
    Logger::log(F("Homing completed 👍"));
    startNextController(CONTROLLER_READY);
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int HomingController::getModeType() const {
  return CONTROLLER_HOMING;
}

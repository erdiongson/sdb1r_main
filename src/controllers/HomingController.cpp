#include "HomingController.h"
#include "../views/MainScreen.h"

HomingController::HomingController(ControllerParams params) : BaseController(params) {}

void HomingController::onStart() {
  Logger::log(F("HomingController::on_start"));

  // Start the homing process
  dispenserHead.x().moveToMin();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMax();

    Profile& profile = profile_manager.getCurrentProfile();
    drawHomingScreen({ profile, { 0, 0, 0, 0 }, 0 });
}

void HomingController::onInteraction(const Interaction& interaction) {
  // Handle "Continue" button pressed in an error dialog
  if (interaction.key_pressed == TAG_CONTINUE) {
    running = true;
    // Restart the homing process
    dispenserHead.x().moveToMin();
    dispenserHead.y().moveToMin();
    dispenserHead.z().moveToMax();

    // Redraw the screen
    Profile& profile = profile_manager.getCurrentProfile();
    drawHomingScreen({ profile, { 0, 0, 0, 0 }, 0 });
  }
}

ControllerStepResult HomingController::onStep() {
  // Early return if not running (error state)
  if (!running) return ControllerStepResult(false);

  DispenserProcessResult result = dispenserHead.process();

  // Handle possible errors
  if (result.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    Profile& profile = profile_manager.getCurrentProfile();
    drawHomingScreen({ profile, { 0, 0, 0, 0 }, DIALOG_ERROR_LIMIT_SWITCH_HOMING });
    running = false;
    return ControllerStepResult(false);
  }

  if (result.steppers == AXIS_STATE_COMPLETE) {
    Logger::log(F("Homing completed "));
    startNextController(CONTROLLER_READY);
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int HomingController::getModeType() const {
  return CONTROLLER_HOMING;
}

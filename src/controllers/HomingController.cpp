#include "HomingController.h"
#include "../views/MainScreen.h"
#include "../serial/PlcSerial.h"
#include "../../Config.h"

HomingController::HomingController(ControllerParams params) : BaseController(params) {}

void HomingController::onStart() {
  Logger::log(F("HomingController::on_start"));

  // Set busy state
  PlcSerial::setBusy(true);

  // Start the homing process
  dispenserHead.moveToHome();
  timeout_at = millis() + HOMING_TIMEOUT * 1000;

    Profile& profile = profile_manager.getCurrentProfile();
    drawHomingScreen({ profile, { 0, 0, 0, 0 }, 0 });
}

void HomingController::onInteraction(const Interaction& interaction) {
  // Handle "Continue" button pressed in an error dialog
  if (interaction.key_pressed == TAG_CONTINUE) {
    running = true;
    // Restart the homing process
    dispenserHead.moveToHome();
    timeout_at = millis() + HOMING_TIMEOUT * 1000;

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

  // Check if homing timed out
  if (timeout_at <= millis()) {
    Logger::log(F("Homing timed out "));
    Profile& profile = profile_manager.getCurrentProfile();
    drawHomingScreen({ profile, { 0, 0, 0, 0 }, DIALOG_ERROR_HOMING_TIMEOUT });
    running = false;
    timeout_at = 0;
    return ControllerStepResult(false);
  }

  // Once all steppers are done, consider homing complete
  if (result.steppers == AXIS_STATE_COMPLETE) {
    Logger::log(F("Homing completed "));
    timeout_at = 0;
    // Clear busy state
    PlcSerial::setBusy(false);
    startNextController(CONTROLLER_READY);
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int HomingController::getModeType() const {
  return CONTROLLER_HOMING;
}

#include "HomingController.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../views/MainScreen.h"

HomingController::HomingController(ControllerParams params)
  : BaseController(params) {}

void HomingController::onStart() {
  Logger::log(F("HomingController::on_start"));

  // If the dispenser head is at the limit switches, clear them before
  // proceeding with the homing process
  Profile& profile = profile_manager.getCurrentProfile();
  drawMainScreen(phost, HOMINGMENU, {profile, 0, 0, 0, 0, 0});
  dispenserHead.clearLimits();
}

void HomingController::onInteraction(const Interaction& interaction) {
  if (interaction.key_pressed == TAG_CONTINUE) {
    Profile& profile = profile_manager.getCurrentProfile();
    drawMainScreen(phost, HOMINGMENU, {profile, 0, 0, 0, 0, 0});
    dispenserHead.clearLimits();
  }
}

ControllerStepResult HomingController::onStep() {
  DispenserProcessResult result = dispenserHead.process();

  // Handle possible errors
  if (result.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    Profile& profile = profile_manager.getCurrentProfile();
    drawMainScreen(phost, HOMINGMENU, {profile, 0, 0, 0, 0, DIALOG_ERROR_LIMIT_SWITCH_HOMING});
    stage = STAGE_ERROR;
    return ControllerStepResult(false);
  }

  if (stage == STAGE_CLEAR && result.steppers == AXIS_STATE_COMPLETE) {
    Logger::log(F("Axis cleared 👍"));
    dispenserHead.x().moveToMin();
    dispenserHead.y().moveToMin();
    dispenserHead.z().moveToMin();
    stage = STAGE_HOME;
    return ControllerStepResult(true);
  }

  if (stage == STAGE_HOME && result.steppers == AXIS_STATE_COMPLETE) {
    Logger::log(F("Axis homed 👍"));
    startNextController(CONTROLLER_READY);
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int HomingController::getModeType() const {
  return CONTROLLER_HOMING;
}

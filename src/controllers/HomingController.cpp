#include "HomingController.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../Utils.h"
#include "../views/MainScreen.h"

HomingController::HomingController(ControllerParams params)
  : BaseController(params) {}

void HomingController::onStart(Profile& profile) {
  Dprint(F("HomingController::on_start"));

  // If the dispenser head is at the limit switches, clear them before
  // proceeding with the homing process
  draw_main_screen(phost, HOMINGMENU);
  dispenserHead.clear_limits();
}

void HomingController::onInteraction(const Interaction& interaction) {
  if (interaction.key_pressed == TAG_CONTINUE) {
    stage = STAGE_CLEAR;
    draw_main_screen(phost, HOMINGMENU);
    dispenserHead.clear_limits();
  }
}

ControllerStepResult HomingController::onStep() {
  DispenserProcessResult result = dispenserHead.process();

  // Handle possible errors
  if (result.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    MainScreenParams params = { 0, 0, 0, 0, DIALOG_ERROR_LIMIT_SWITCH };
    draw_main_screen(phost, HOMINGMENU, &params);
    stage = STAGE_ERROR;
    return ControllerStepResult(-1, -1);
  }

  if (stage == STAGE_CLEAR && result.steppers == AXIS_STATE_COMPLETE) {
    Dprint(F("Axis cleared 👍"));
    dispenserHead.x().moveToMin();
    dispenserHead.y().moveToMin();
    dispenserHead.z().moveToMin();
    stage = STAGE_HOME;
    return ControllerStepResult(-1, -1);
  }

  if (stage == STAGE_HOME && result.steppers == AXIS_STATE_COMPLETE) {
    Dprint(F("Axis homed 👍"));
    startNextController(CONTROLLER_READY);
  }

  return ControllerStepResult(-1, -1);
}

int HomingController::getModeType() const {
  return CONTROLLER_HOMING;
}

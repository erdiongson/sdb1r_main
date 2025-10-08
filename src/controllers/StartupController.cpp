#include "StartupController.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../Utils.h"
#include "../views/LogoScreen.h"

StartupController::StartupController(ControllerParams params)
  : BaseController(params), dispenserHead(params.head) {
  }

void StartupController::on_start(Profile& profile) {
  Dprint(F("StartupController::on_start"));
  draw_logo_screen(phost, 0);
  dispenserHead.send_handshake();
}

void StartupController::on_interaction(const Interaction& interaction) {}

ControllerStepResult StartupController::on_step() {
  DispenserProcessResult result = dispenserHead.process();

  // Handle errors
  if (result.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    draw_logo_screen(phost, ERROR_ACK_ERROR);
    stage = STAGE_ERROR;
    return ControllerStepResult(-1, -1);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    draw_logo_screen(phost, ERROR_MARKER_NOT_DETECTED);
    stage = STAGE_ERROR;
    return ControllerStepResult(-1, -1);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    draw_logo_screen(phost, ERROR_IR_SENSOR);
    stage = STAGE_ERROR;
    return ControllerStepResult(-1, -1);
  }
  if (result.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    draw_logo_screen(phost, ERROR_LIMIT_SWITCH);
    stage = STAGE_ERROR;
    return ControllerStepResult(-1, -1);
  }

  if (stage == STAGE_HANDSHAKE && result.dispenser == DISPENSER_STATE_IDLING) {
    Dprint(F("Handshake acknowledged 👍"));
    if (dispenserHead.x().isAtMin()) dispenserHead.x().moveBy(STEPS_PER_UNIT_X * 30);
    if (dispenserHead.y().isAtMin()) dispenserHead.y().moveBy(STEPS_PER_UNIT_Y * 30);
    if (dispenserHead.z().isAtMin()) dispenserHead.z().moveBy(STEPS_PER_UNIT_Z * 30);
    
    stage = STAGE_CLEAR;
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
    start_next_controller(CONTROLLER_HOME);
  }
    
  return ControllerStepResult(-1, -1);
}

int StartupController::get_mode_type() const {
  return CONTROLLER_HOME;
}

#include "StartupController.h"
#include "../views/LogoScreen.h"
#include "../views/MainScreen.h"

StartupController::StartupController(ControllerParams params) : BaseController(params), dispenserHead(params.head) {}

void StartupController::onStart() {
  Logger::log(F("StartupController::on_start"));

  // Load profile from EEPROM
  Logger::log(F("Loading profile.."));
  profile_manager.loadProfile();

  drawLogoScreen(phost, 0);

  // Check if dispenser is online and responding
  dispenserHead.sendHandshake();
}

void StartupController::onInteraction(const Interaction& interaction) {}

 ControllerStepResult StartupController::onStep() {
  DispenserProcessResult result = dispenserHead.process();

  // Handle possible errors
  if (result.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    drawLogoScreen(phost, DIALOG_ERROR_ACK_ERROR);
    stage = STAGE_ERROR;
    return ControllerStepResult(false);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    drawLogoScreen(phost, DIALOG_ERROR_MARKER_NOT_DETECTED);
    stage = STAGE_ERROR;
    return ControllerStepResult(false);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    drawLogoScreen(phost, DIALOG_ERROR_IR_SENSOR);
    stage = STAGE_ERROR;
    return ControllerStepResult(false);
  }
  if (result.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    drawLogoScreen(phost, DIALOG_ERROR_LIMIT_SWITCH_HOMING);
    stage = STAGE_ERROR;
    return ControllerStepResult(false);
  }

  if (stage == STAGE_HANDSHAKE && result.dispenser == DISPENSER_STATE_IDLING) {
    Logger::log(F("Handshake acknowledged 👍"));
    drawLogoScreen(phost, 0);
    dispenserHead.clearLimits();
    stage = STAGE_CLEAR;
    return ControllerStepResult(false);
  }

  if (stage == STAGE_CLEAR && result.steppers == AXIS_STATE_COMPLETE) {
    Logger::log(F("Axis cleared 👍"));
    dispenserHead.x().moveToMin();
    dispenserHead.y().moveToMin();
    dispenserHead.z().moveToMax();
    stage = STAGE_HOME;
    return ControllerStepResult(true);
  }

  if (stage == STAGE_HOME && result.steppers == AXIS_STATE_COMPLETE) {
    Logger::log(F("Homing completed 👍"));
    startNextController(CONTROLLER_READY);
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int StartupController::getModeType() const {
  return CONTROLLER_STARTUP;
}

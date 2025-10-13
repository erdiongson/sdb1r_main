#include "StartupController.h"
#include "../views/LogoScreen.h"

StartupController::StartupController(ControllerParams params) : BaseController(params), dispenserHead(params.head) {}

void StartupController::onStart() {
  Logger::log(F("StartupController::on_start"));

  // Load profile from EEPROM
  Logger::log("Loading profile..");
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
  if (result.dispenser == DISPENSER_STATE_ERROR_CYCLES_TIMEOUT) {
    drawLogoScreen(phost, DIALOG_ERROR_CYCLE_TIMEOUT);
    stage = STAGE_ERROR;
    return ControllerStepResult(false);
  }

  if (stage == STAGE_HANDSHAKE && result.dispenser == DISPENSER_STATE_IDLING) {
    Logger::log(F("Handshake acknowledged 👍"));
    startNextController(CONTROLLER_HOMING);
  }

  return ControllerStepResult(false);
}

int StartupController::getModeType() const {
  return CONTROLLER_STARTUP;
}

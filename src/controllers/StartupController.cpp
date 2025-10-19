#include "StartupController.h"
#include "../views/LogoScreen.h"
#include "../views/MainScreen.h"
#include "../views/ViewCommon.h"

StartupController::StartupController(ControllerParams params) : BaseController(params), dispenserHead(params.head) {}

void StartupController::onStart() {
  Logger::log(F("StartupController::on_start"));

  // Load profile from EEPROM
  Logger::log(F("Loading profile.."));
  profile_manager.loadProfile();

  logoParams = {0, PROGMEM_STR(F("Initializing.."))};
  drawLogoScreen(phost, logoParams);
  delay(500);

  // Check if dispenser is online and responding
  dispenserHead.sendHandshake();
  logoParams.status_message = PROGMEM_STR(F("Pending Response.."));
  drawLogoScreen(phost, logoParams);
}

void StartupController::onInteraction(const Interaction& interaction) {
  // Handle "Continue" button pressed in an error dialog
  if (interaction.key_pressed == TAG_CONTINUE) {
    logoParams.dialog_code = 0;
    running = true;

    if (stage == STAGE_CLEAR) {
      logoParams.status_message = PROGMEM_STR(F("Unlatching.."));
      drawLogoScreen(phost, logoParams);
      dispenserHead.clearLimits();
    } else if (stage == STAGE_HOME) {
      logoParams.status_message = PROGMEM_STR(F("Homing.."));
      drawLogoScreen(phost, logoParams);
      dispenserHead.x().moveToMin();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMax();
    }
  }
}

 ControllerStepResult StartupController::onStep() {
  // Early return if not running (error state)
  if (!running) return ControllerStepResult(false);

  DispenserProcessResult result = dispenserHead.process();

  // Handle possible errors
  if (result.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    logoParams.dialog_code = DIALOG_ERROR_ACK_ERROR;
    drawLogoScreen(phost, logoParams);
    running = false;
    return ControllerStepResult(false);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    logoParams.dialog_code = DIALOG_ERROR_MARKER_NOT_DETECTED;
    drawLogoScreen(phost, logoParams);
    running = false;
    return ControllerStepResult(false);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    logoParams.dialog_code = DIALOG_ERROR_IR_SENSOR;
    drawLogoScreen(phost, logoParams);
    running = false;
    return ControllerStepResult(false);
  }
  if (result.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    logoParams.dialog_code = DIALOG_ERROR_LIMIT_SWITCH_HOMING;
    drawLogoScreen(phost, logoParams);
    running = false;
    return ControllerStepResult(false);
  }

  if (stage == STAGE_HANDSHAKE && result.dispenser == DISPENSER_STATE_IDLING) {
    Logger::log(F("Handshake acknowledged 👍"));
    logoParams.status_message = "Unlatching..";
    drawLogoScreen(phost, logoParams);
    dispenserHead.clearLimits();
    stage = STAGE_CLEAR;
    return ControllerStepResult(false);
  }

  if (stage == STAGE_CLEAR && result.steppers == AXIS_STATE_COMPLETE) {
    Logger::log(F("Axis cleared 👍"));
    logoParams.status_message = "Homing..";
    drawLogoScreen(phost, logoParams);
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

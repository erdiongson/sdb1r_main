#include "StartupController.h"
#include "../views/LogoScreen.h"
#include "../views/MainScreen.h"
#include "../views/ViewCommon.h"
#include "../../Config.h"

StartupController::StartupController(ControllerParams params) : BaseController(params), dispenserHead(params.head) {}

void StartupController::onStart() {
  Logger::log(F("StartupController::on_start"));

  // Load profile from EEPROM
  Logger::log(F("Loading profile.."));
  profile_manager.loadProfile();

  logo_params = {0, PROGMEM_STR_CONCAT(F("%s - %s"), FWVERM, F("Starting.."))};
  drawLogoScreen(phost, logo_params);
  delay(500);

  // Check if dispenser is online and responding
  dispenserHead.sendHandshake();
  logo_params.status_message = PROGMEM_STR_CONCAT(F("%s - %s"), FWVERM, F("Pending Response.."));
  drawLogoScreen(phost, logo_params);
}

void StartupController::onInteraction(const Interaction& interaction) {
  // Handle "Continue" button pressed in an error dialog
  if (interaction.key_pressed == TAG_CONTINUE) {
    logo_params.dialog_code = 0;
    running = true;

    if (stage == STAGE_CLEAR) {
      logo_params.status_message = PROGMEM_STR_CONCAT(F("%s - %s"), FWVERM, F("Unlatching.."));
      drawLogoScreen(phost, logo_params);
      dispenserHead.clearLimits();
    } else if (stage == STAGE_HOME) {
      logo_params.status_message = PROGMEM_STR_CONCAT(F("%s - %s"), FWVERM, F("Homing.."));
      drawLogoScreen(phost, logo_params);
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
    logo_params.dialog_code = DIALOG_ERROR_ACK_ERROR;
    drawLogoScreen(phost, logo_params);
    running = false;
    return ControllerStepResult(false);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    logo_params.dialog_code = DIALOG_ERROR_MARKER_NOT_DETECTED;
    drawLogoScreen(phost, logo_params);
    running = false;
    return ControllerStepResult(false);
  }
  if (result.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    logo_params.dialog_code = DIALOG_ERROR_IR_SENSOR;
    drawLogoScreen(phost, logo_params);
    running = false;
    return ControllerStepResult(false);
  }
  if (result.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    logo_params.dialog_code = DIALOG_ERROR_LIMIT_SWITCH_HOMING;
    drawLogoScreen(phost, logo_params);
    running = false;
    return ControllerStepResult(false);
  }

  if (stage == STAGE_HANDSHAKE && result.dispenser == DISPENSER_STATE_IDLING) {
    Logger::log(F("Handshake acknowledged 👍"));
    logo_params.status_message = PROGMEM_STR_CONCAT(F("%s - %s"), FWVERM, F("Unlatching.."));
    drawLogoScreen(phost, logo_params);
    dispenserHead.clearLimits();
    stage = STAGE_CLEAR;
    return ControllerStepResult(false);
  }

  if (stage == STAGE_CLEAR && result.steppers == AXIS_STATE_COMPLETE) {
    // If any axis is not at the expected position, consider it failed to clear
    if (dispenserHead.x().isAtMin() || dispenserHead.y().isAtMin() || (dispenserHead.z().isAtMax() && !Z_UNLATCH_SKIP)) {
      Logger::log(F("Axis failed to clear 👎"));
      logo_params.dialog_code = DIALOG_ERROR_UNLATCH_ERROR;
      drawLogoScreen(phost, logo_params);
      running = false;
      return ControllerStepResult(false);
    }

    Logger::log(F("Axis cleared 👍"));
    logo_params.status_message = PROGMEM_STR_CONCAT(F("%s - %s"), FWVERM, F("Homing.."));
    drawLogoScreen(phost, logo_params);
    dispenserHead.x().moveToMin();
    dispenserHead.y().moveToMin();
    dispenserHead.z().moveToMax();
    timeout_at = millis() + HOMING_TIMEOUT * 1000;
    stage = STAGE_HOME;
    return ControllerStepResult(true);
  }

  if (stage == STAGE_HOME) {

    // Check if homing timed out
    if (timeout_at <= millis()) {
      Logger::log(F("Homing timed out 👎"));
      logo_params.dialog_code = DIALOG_ERROR_HOMING_TIMEOUT;
      drawLogoScreen(phost, logo_params);
      running = false;
      timeout_at = 0;
      return ControllerStepResult(false);
    }

    // Once all steppers are done, consider homing complete
    if (result.steppers == AXIS_STATE_COMPLETE) {
      Logger::log(F("Homing completed 👍"));
      startNextController(CONTROLLER_READY);
    }
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int StartupController::getModeType() const {
  return CONTROLLER_STARTUP;
}

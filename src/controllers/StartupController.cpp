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
  
  // Start the init stage timer
  stage = STAGE_INIT;
  stage_start_time = millis();
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
      dispenserHead.moveToHome();
    }
  }
}

 ControllerStepResult StartupController::onStep() {
  // Early return if not running (error state)
  if (!running) return ControllerStepResult(false);

  // Secret debug access sequence - check for arbitrary screen touches
  if (stage == STAGE_INIT || stage == STAGE_DEBUG_1) {
    // Read touch coordinates directly
    uint32_t touch = Gpu_Hal_Rd32(phost, REG_TOUCH_SCREEN_XY);
    uint16_t x = (uint16_t)(touch >> 16);
    uint16_t y = (uint16_t)(touch & 0xFFFF);
    
    // Check if there's a valid touch (coordinates != 0x8000 means touch is present)
    if (x != 0x8000 && y != 0x8000) {
        Logger::log(F("Debug sequence: check true"));
      if (stage == STAGE_INIT) {
        // Check for top-left touch (x < 80, y < 80)
        if (x < 80 && y < 80) {
          Logger::log(F("Debug sequence: Step 1 activated"));
          stage = STAGE_DEBUG_1;
          stage_start_time = millis();
          logo_params.status_message = "";
          drawLogoScreen(phost, logo_params);
          return ControllerStepResult(true);
        }
      } else if (stage == STAGE_DEBUG_1) {
        // Check for top-right touch (x > 270, y < 80)
        if (x > 270 && y < 80) {
          Logger::log(F("Debug sequence: Step 2 activated - entering debug mode"));
          startNextController(CONTROLLER_DEBUG);
          return ControllerStepResult(false);
        }
      }
    }
  }

  // Handle STAGE_INIT - wait 500ms before proceeding to handshake
  if (stage == STAGE_INIT) {
    if (millis() - stage_start_time >= 1000) {
      // 500ms has elapsed, move to handshake stage
      stage = STAGE_HANDSHAKE;
      dispenserHead.sendHandshake();
      logo_params.status_message = PROGMEM_STR_CONCAT(F("%s - %s"), FWVERM, F("Pending Response.."));
      drawLogoScreen(phost, logo_params);
    }
    // Still in init stage, keep processing
    return ControllerStepResult(true);
  }

  // Handle STAGE_DEBUG_1 - wait 3s before proceeding to handshake
  if (stage == STAGE_DEBUG_1) {
    if (millis() - stage_start_time >= 3000) {
      // 3s has elapsed without completing debug sequence, proceed normally
      Logger::log(F("Debug sequence timed out - proceeding to handshake"));
      stage = STAGE_HANDSHAKE;
      dispenserHead.sendHandshake();
      logo_params.status_message = PROGMEM_STR_CONCAT(F("%s - %s"), FWVERM, F("Pending Response.."));
      drawLogoScreen(phost, logo_params);
    }
    // Still in debug_1 stage, keep processing
    return ControllerStepResult(true);
  }

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
    if (dispenserHead.checkCleared()) {
      Logger::log(F("Axis failed to clear 👎"));
      logo_params.dialog_code = DIALOG_ERROR_UNLATCH_ERROR;
      drawLogoScreen(phost, logo_params);
      running = false;
      return ControllerStepResult(false);
    }

    Logger::log(F("Axis cleared 👍"));
    logo_params.status_message = PROGMEM_STR_CONCAT(F("%s - %s"), FWVERM, F("Homing.."));
    drawLogoScreen(phost, logo_params);
    dispenserHead.moveToHome();
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

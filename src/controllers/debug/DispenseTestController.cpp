#include "DispenseTestController.h"
#include "../../views/debug/DispenseTestScreen.h"

DispenseTestController::DispenseTestController(ControllerParams params) : BaseController(params) {}

void DispenseTestController::onStart() {
  Logger::log(F("DispenseTestController::on_start"));

  drawDispenseTestScreen(phost, { "READY" });
}

void DispenseTestController::onInteraction(const Interaction& interaction) {
  int button = interaction.key_pressed;

  switch (button) {
    case TAG_DISPENSE_BACK:
      startNextController(CONTROLLER_DEBUG);
      break;

    case TAG_DISPENSE: {
      Logger::log(F("DispenseTestController: Sending Dispense Command"));
      drawDispenseTestScreen(phost, { "SENT DISPENSE COMMAND" });
      dispenserHead.sendDispense();
      state = WAITING_FOR_RESPONSE;
      break;
    }

    // Vibration level buttons (U0-U4)
    case TAG_VIB_U0: {
      Logger::log(F("DispenseTestController: Vibration level set to U0"));
      drawDispenseTestScreen(phost, { "SETTING VIB LEVEL U0" });
      dispenserHead.setVibrationLevel(0);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U1: {
      Logger::log(F("DispenseTestController: Vibration level set to U1"));
      drawDispenseTestScreen(phost, { "SETTING VIB LEVEL U1" });
      dispenserHead.setVibrationLevel(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U2: {
      Logger::log(F("DispenseTestController: Vibration level set to U2"));
      drawDispenseTestScreen(phost, { "SETTING VIB LEVEL U2" });
      dispenserHead.setVibrationLevel(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U3: {
      Logger::log(F("DispenseTestController: Vibration level set to U3"));
      drawDispenseTestScreen(phost, { "SETTING VIB LEVEL U3" });
      dispenserHead.setVibrationLevel(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U4: {
      Logger::log(F("DispenseTestController: Vibration level set to U4"));
      drawDispenseTestScreen(phost, { "SETTING VIB LEVEL U4" });
      dispenserHead.setVibrationLevel(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    // Vibration time buttons (1-5s)
    case TAG_VIB_TIME_1: {
      Logger::log(F("DispenseTestController: Vibration time set to 1s"));
      drawDispenseTestScreen(phost, { "SETTING VIB TIME 1s" });
      dispenserHead.setVibrationTime(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_2: {
      Logger::log(F("DispenseTestController: Vibration time set to 2s"));
      drawDispenseTestScreen(phost, { "SETTING VIB TIME 2s" });
      dispenserHead.setVibrationTime(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_3: {
      Logger::log(F("DispenseTestController: Vibration time set to 3s"));
      drawDispenseTestScreen(phost, { "SETTING VIB TIME 3s" });
      dispenserHead.setVibrationTime(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_4: {
      Logger::log(F("DispenseTestController: Vibration time set to 4s"));
      drawDispenseTestScreen(phost, { "SETTING VIB TIME 4s" });
      dispenserHead.setVibrationTime(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_5: {
      Logger::log(F("DispenseTestController: Vibration time set to 5s"));
      drawDispenseTestScreen(phost, { "SETTING VIB TIME 5s" });
      dispenserHead.setVibrationTime(5);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    default:
      break;
  }
}

ControllerStepResult DispenseTestController::onStep() {
  DispenserProcessResult result = dispenserHead.process();

  if (state == WAITING_FOR_RESPONSE && result.dispenser == DISPENSER_STATE_ACKNOWLEDGED) {
    drawDispenseTestScreen(phost, { "ACKNOWLEGED" });
    return ControllerStepResult(false);
  }

  if (state == WAITING_FOR_RESPONSE) {
    const char* message = "";
    switch (result.dispenser) {
      case DISPENSER_STATE_IDLING:
        message = "RESPONSE RECEIVED";
        break;
      case DISPENSER_STATE_ERROR_ACK_ERROR:
        message = "ACK ERROR";
        break;
      case DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE:
        message = "IR SENSOR FAILURE";
        break;
      case DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED:
        message = "MARKER NOT DETECTED";
        break;
      case DISPENSER_STATE_ERROR_CYCLES_TIMEOUT:
        message = "CYCLE TIMEOUT";
        break;
      default:
        break;
    }

    state = RECEIVED_RESPONSE;
    drawDispenseTestScreen(phost, { message });
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int DispenseTestController::getModeType() const {
  return CONTROLLER_DISPENSE_TEST;
}
#include "DispenseTestController.h"
#include "../../views/debug/DispenseTestScreen.h"
#include "../../views/common/Keyboards.h"

DispenseTestController::DispenseTestController(ControllerParams params) : BaseController(params) {}

void DispenseTestController::onStart() {
  Logger::log(F("DispenseTestController::on_start"));

  updateScreen("READY");
}

void DispenseTestController::updateScreen(const char* status_message) {
  DispenseTestScreenParams params;
  params.status_message = status_message;
  params.repeat_count = repeat_count;

  params.is_dispensing = repeat_state.enabled;
  params.current_repeat_count = repeat_state.current_count + 1;
  params.total_repeat_count = repeat_state.total_count;
  drawDispenseTestScreen(phost, params);
}

void DispenseTestController::onInteraction(const Interaction& interaction) {
  int button = interaction.key_pressed;

  switch (button) {
    case TAG_DISPENSE_BACK:
      startNextController(CONTROLLER_DEBUG);
      break;

    case TAG_DISPENSE: {
      Logger::log(F("DispenseTestController: Sending Dispense Command"));
      if (repeat_count > 0) {
        repeat_state.enabled = true;
        repeat_state.current_count = 0;
        repeat_state.total_count = repeat_count;
      } else {
        repeat_state.enabled = false;
        repeat_state.current_count = 0;
        repeat_state.total_count = 0;
      }
      updateScreen("SENT DISPENSE");
      dispenserHead.sendDispense();
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_DISPENSE_STOP: {
      Logger::log(F("DispenseTestController: Stopping dispense"));
      repeat_state.enabled = false;
      repeat_state.current_count = 0;
      repeat_state.total_count = 0;
      break;
    }

    case TAG_DISPENSE_REPEAT: {
      Logger::log(F("DispenseTestController: Set repeat count"));
      repeat_count = (int)getKeypadValue(phost, repeat_count, MIN_DISPENSE_REPEAT_COUNT, MAX_DISPENSE_REPEAT_COUNT, false);
      updateScreen("READY");
      break;
    }

    // Vibration level buttons (U0-U4)
    case TAG_VIB_U0: {
      Logger::log(F("DispenseTestController: Vibration level set to U0"));
      updateScreen("SETTING VIB LEVEL U0");
      dispenserHead.setVibrationLevel(0);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U1: {
      Logger::log(F("DispenseTestController: Vibration level set to U1"));
      updateScreen("SETTING VIB LEVEL U1");
      dispenserHead.setVibrationLevel(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U2: {
      Logger::log(F("DispenseTestController: Vibration level set to U2"));
      updateScreen("SETTING VIB LEVEL U2");
      dispenserHead.setVibrationLevel(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U3: {
      Logger::log(F("DispenseTestController: Vibration level set to U3"));
      updateScreen("SETTING VIB LEVEL U3");
      dispenserHead.setVibrationLevel(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U4: {
      Logger::log(F("DispenseTestController: Vibration level set to U4"));
      updateScreen("SETTING VIB LEVEL U4");
      dispenserHead.setVibrationLevel(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    // Vibration time buttons (1-5s)
    case TAG_VIB_TIME_1: {
      Logger::log(F("DispenseTestController: Vibration time set to 1s"));
      updateScreen("SETTING VIB TIME 1s");
      dispenserHead.setVibrationTime(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_2: {
      Logger::log(F("DispenseTestController: Vibration time set to 2s"));
      updateScreen("SETTING VIB TIME 2s");
      dispenserHead.setVibrationTime(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_3: {
      Logger::log(F("DispenseTestController: Vibration time set to 3s"));
      updateScreen("SETTING VIB TIME 3s");
      dispenserHead.setVibrationTime(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_4: {
      Logger::log(F("DispenseTestController: Vibration time set to 4s"));
      updateScreen("SETTING VIB TIME 4s");
      dispenserHead.setVibrationTime(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_5: {
      Logger::log(F("DispenseTestController: Vibration time set to 5s"));
      updateScreen("SETTING VIB TIME 5s");
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
    updateScreen("ACKNOWLEGED");
    return ControllerStepResult(false);
  }

  if (state == WAITING_FOR_RESPONSE && result.dispenser != DISPENSER_STATE_SENT) {
    const char* message = "";
    bool is_error = false;
    
    switch (result.dispenser) {
      case DISPENSER_STATE_IDLING:
        message = "COMPLETED";
        break;
      case DISPENSER_STATE_ERROR_ACK_ERROR:
        message = "ACK TIMEOUT";
        is_error = true;
        break;
      case DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE:
        message = "IR SENSOR FAILURE";
        is_error = true;
        break;
      case DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED:
        message = "MARKER NOT DETECTED";
        is_error = true;
        break;
      case DISPENSER_STATE_ERROR_CYCLES_TIMEOUT:
        message = "CYCLE TIMEOUT";
        is_error = true;
        break;
      default:
        break;
    }

    state = RECEIVED_RESPONSE;
    
    // Handle repeat dispense logic (similar to bounce)
    if (!is_error && repeat_state.enabled) {
      // Check if there are more dispenses to do
      if (repeat_state.current_count < repeat_state.total_count) {
        // Increment and send next dispense command
        repeat_state.current_count++;
        updateScreen("SENT DISPENSE");
        dispenserHead.sendDispense();
        state = WAITING_FOR_RESPONSE;
      } else {
        // Last dispense completed - reset and show completion
        updateScreen("ALL DONE");
      }
    } else {
      // Error occurred or single dispense completed
      repeat_state.enabled = false;
      updateScreen(message);
    }
  }

  // Update screen at regular intervals
  static unsigned long last_update_time = 0;
  unsigned long current_time = millis();
  if (current_time - last_update_time >= DISPENSE_TEST_REFRESH_INTERVAL_MS) {
    last_update_time = current_time;
    if (state == WAITING_FOR_RESPONSE) {
      updateScreen("WAITING...");
    }
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int DispenseTestController::getModeType() const {
  return CONTROLLER_DISPENSE_TEST;
}
#include "DispenseTestController.h"
#include "../../views/debug/DispenseTestScreen.h"
#include "../../views/common/Keyboards.h"
#include "../../views/ViewCommon.h"

DispenseTestController::DispenseTestController(ControllerParams params) : BaseController(params) {}

void DispenseTestController::onStart() {
  Logger::log(F("DispenseTestController::on_start"));

  updateScreen(PROGMEM_STR(F("READY")));
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
      updateScreen(PROGMEM_STR(F("SENT DISPENSE")));
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
      updateScreen("SET U0");
      dispenserHead.setVibrationLevel(0);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U1: {
      Logger::log(F("DispenseTestController: Vibration level set to U1"));
      updateScreen("SET U1");
      dispenserHead.setVibrationLevel(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U2: {
      Logger::log(F("DispenseTestController: Vibration level set to U2"));
      updateScreen("SET U2");
      dispenserHead.setVibrationLevel(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U3: {
      Logger::log(F("DispenseTestController: Vibration level set to U3"));
      updateScreen("SET U3");
      dispenserHead.setVibrationLevel(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U4: {
      Logger::log(F("DispenseTestController: Vibration level set to U4"));
      updateScreen("SET U4");
      dispenserHead.setVibrationLevel(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    // Vibration time buttons (1-5s)
    case TAG_VIB_TIME_1: {
      Logger::log(F("DispenseTestController: Vibration time set to 1s"));
      updateScreen("SET 1s");
      dispenserHead.setVibrationTime(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_2: {
      Logger::log(F("DispenseTestController: Vibration time set to 2s"));
      updateScreen("SET 2s");
      dispenserHead.setVibrationTime(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_3: {
      Logger::log(F("DispenseTestController: Vibration time set to 3s"));
      updateScreen("SET 3s");
      dispenserHead.setVibrationTime(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_4: {
      Logger::log(F("DispenseTestController: Vibration time set to 4s"));
      updateScreen("SET 4s");
      dispenserHead.setVibrationTime(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_5: {
      Logger::log(F("DispenseTestController: Vibration time set to 5s"));
      updateScreen("SET 5s");
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
    updateScreen(PROGMEM_STR(F("ACK")));
    return ControllerStepResult(false);
  }

  if (state == WAITING_FOR_RESPONSE && result.dispenser != DISPENSER_STATE_SENT) {
    const char* message = nullptr;
    bool is_error = false;
    
    switch (result.dispenser) {
      case DISPENSER_STATE_IDLING:
        message = PROGMEM_STR(F("COMPLETED"));
        break;
      case DISPENSER_STATE_ERROR_ACK_ERROR:
        message = PROGMEM_STR(F("ACK TIMEOUT"));
        is_error = true;
        break;
      case DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE:
        message = PROGMEM_STR(F("IR SENSOR FAILURE"));
        is_error = true;
        break;
      case DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED:
        message = PROGMEM_STR(F("MARKER NOT DETECTED"));
        is_error = true;
        break;
      case DISPENSER_STATE_ERROR_CYCLES_TIMEOUT:
        message = PROGMEM_STR(F("CYCLE TIMEOUT"));
        is_error = true;
        break;
      default:
        message = PROGMEM_STR(F(""));
        break;
    }

    state = RECEIVED_RESPONSE;
    
    // Handle repeat dispense logic (similar to bounce)
    if (!is_error && repeat_state.enabled) {
      // Check if there are more dispenses to do
      if (repeat_state.current_count < repeat_state.total_count) {
        // Increment and send next dispense command
        repeat_state.current_count++;
        updateScreen(PROGMEM_STR(F("SENT DISPENSE")));
        dispenserHead.sendDispense();
        state = WAITING_FOR_RESPONSE;
      } else {
        // Last dispense completed - reset and show completion
        updateScreen(PROGMEM_STR(F("ALL DONE")));
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
      updateScreen(PROGMEM_STR(F("WAITING..")));
    }
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int DispenseTestController::getModeType() const {
  return CONTROLLER_DISPENSE_TEST;
}
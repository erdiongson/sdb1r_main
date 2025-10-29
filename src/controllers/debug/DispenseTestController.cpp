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
  // Copy message to persistent buffer to avoid g_view_temp_buffer conflicts
  // Check if message is already in our buffer to avoid unnecessary copy
  if (status_message != status_message_buffer) {
    strncpy(status_message_buffer, status_message, sizeof(status_message_buffer) - 1);
    status_message_buffer[sizeof(status_message_buffer) - 1] = '\0';
  }
  
  DispenseTestScreenParams params;
  params.status_message = status_message_buffer;
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
      delay(100);
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
      delay(100);
      dispenserHead.setVibrationLevel(0);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U1: {
      Logger::log(F("DispenseTestController: Vibration level set to U1"));
      updateScreen("SET U1");
      delay(100);
      dispenserHead.setVibrationLevel(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U2: {
      Logger::log(F("DispenseTestController: Vibration level set to U2"));
      updateScreen("SET U2");
      delay(100);
      dispenserHead.setVibrationLevel(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U3: {
      Logger::log(F("DispenseTestController: Vibration level set to U3"));
      updateScreen("SET U3");
      delay(100);
      dispenserHead.setVibrationLevel(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_U4: {
      Logger::log(F("DispenseTestController: Vibration level set to U4"));
      updateScreen("SET U4");
      delay(100);
      dispenserHead.setVibrationLevel(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    // Vibration time buttons (1-5s)
    case TAG_VIB_TIME_1: {
      Logger::log(F("DispenseTestController: Vibration time set to 1s"));
      updateScreen("SET 1s");
      delay(100);
      dispenserHead.setVibrationTime(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_2: {
      Logger::log(F("DispenseTestController: Vibration time set to 2s"));
      updateScreen("SET 2s");
      delay(100);
      dispenserHead.setVibrationTime(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_3: {
      Logger::log(F("DispenseTestController: Vibration time set to 3s"));
      updateScreen("SET 3s");
      delay(100);
      dispenserHead.setVibrationTime(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_4: {
      Logger::log(F("DispenseTestController: Vibration time set to 4s"));
      updateScreen("SET 4s");
      delay(100);
      dispenserHead.setVibrationTime(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_VIB_TIME_5: {
      Logger::log(F("DispenseTestController: Vibration time set to 5s"));
      updateScreen("SET 5s");
      delay(100);
      dispenserHead.setVibrationTime(5);
      state = WAITING_FOR_RESPONSE;
      break;
    }

    case TAG_GET_FW_VERSION: {
      Logger::log(F("DispenseTestController: Querying firmware version"));
      updateScreen("QUERYING FW..");
      delay(100);
      dispenserHead.queryFirmwareVersion();
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
    updateScreen("ACK");
    return ControllerStepResult(false);
  }

  if (state == WAITING_FOR_RESPONSE && result.dispenser != DISPENSER_STATE_SENT) {
    bool is_error = false;
    
    switch (result.dispenser) {
      case DISPENSER_STATE_IDLING: {
        // Check if this was a firmware version query
        FirmwareVersionData fw_data = dispenserHead.getFirmwareVersionData();
        if (fw_data.valid) {
          // Format into controller's persistent buffer
          snprintf_P(status_message_buffer, sizeof(status_message_buffer), PSTR("FW:%d"), fw_data.firmware_version);
        } else {
          strcpy_P(status_message_buffer, PSTR("COMPLETED"));
        }
        break;
      }
      case DISPENSER_STATE_ERROR_ACK_ERROR:
        strcpy_P(status_message_buffer, PSTR("ACK TIMEOUT"));
        is_error = true;
        break;
      case DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE:
        strcpy_P(status_message_buffer, PSTR("IR SENSOR FAILURE"));
        is_error = true;
        break;
      case DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED:
        strcpy_P(status_message_buffer, PSTR("MARKER NOT DETECTED"));
        is_error = true;
        break;
      case DISPENSER_STATE_ERROR_CYCLES_TIMEOUT:
        strcpy_P(status_message_buffer, PSTR("CYCLE TIMEOUT"));
        is_error = true;
        break;
      default:
        status_message_buffer[0] = '\0';
        break;
    }

    state = RECEIVED_RESPONSE;
    
    // Handle repeat dispense logic (similar to bounce)
    if (!is_error && repeat_state.enabled) {
      // Check if there are more dispenses to do
      if (repeat_state.current_count < repeat_state.total_count) {
        // Increment and send next dispense command
        repeat_state.current_count++;
        strcpy_P(status_message_buffer, PSTR("SENT DISPENSE"));
        updateScreen(status_message_buffer);
        dispenserHead.sendDispense();
        state = WAITING_FOR_RESPONSE;
      } else {
        // Last dispense completed - reset and show completion
        strcpy_P(status_message_buffer, PSTR("ALL DONE"));
        updateScreen(status_message_buffer);
      }
    } else {
      // Error occurred or single dispense completed
      repeat_state.enabled = false;
      updateScreen(status_message_buffer);
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
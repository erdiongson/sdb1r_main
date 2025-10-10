#include "DispenseTestController.h"
#include "../../views/debug/DispenseTestScreen.h"
#include "../../Constants.h"
#include "../../Utils.h"

DispenseTestController::DispenseTestController(ControllerParams params)
  : BaseController(params) {}

void DispenseTestController::on_start(Profile& profile) {
  Dprint(F("DispenseTestController::on_start"));
  
  DispenseTestScreenParams params = {"Ready"};
  draw_dispense_test_screen(phost, params);
}

void DispenseTestController::on_interaction(const Interaction& interaction) {
  int button = interaction.key_pressed;
  
  switch (button) {
    case TAG_DISPENSE_BACK:
      start_next_controller(CONTROLLER_DEBUG);
      break;
    
    case TAG_DISPENSE: {
      Dprint(F("DispenseTestController: Sending Dispense Command"));
      DispenseTestScreenParams params = {"SENT DISPENSE COMMAND"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.send_dispense();
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    // Vibration level buttons (U0-U4)
    case TAG_VIB_U0: {
      Dprint(F("DispenseTestController: Vibration level set to U0"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U0"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_level(0);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_U1: {
      Dprint(F("DispenseTestController: Vibration level set to U1"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U1"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_level(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_U2: {
      Dprint(F("DispenseTestController: Vibration level set to U2"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U2"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_level(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_U3: {
      Dprint(F("DispenseTestController: Vibration level set to U3"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U3"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_level(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_U4: {
      Dprint(F("DispenseTestController: Vibration level set to U4"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U4"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_level(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    // Vibration time buttons (1-5s)
    case TAG_VIB_TIME_1: {
      Dprint(F("DispenseTestController: Vibration time set to 1s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 1s"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_time(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_TIME_2: {
      Dprint(F("DispenseTestController: Vibration time set to 2s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 2s"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_time(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_TIME_3: {
      Dprint(F("DispenseTestController: Vibration time set to 3s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 3s"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_time(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_TIME_4: {
      Dprint(F("DispenseTestController: Vibration time set to 4s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 4s"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_time(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_TIME_5: {
      Dprint(F("DispenseTestController: Vibration time set to 5s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 5s"};
      draw_dispense_test_screen(phost, params);
      dispenserHead.set_vibration_time(5);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    default:
      break;
  }
}

ControllerStepResult DispenseTestController::on_step() {
  DispenserProcessResult result = dispenserHead.process();

  if (state == WAITING_FOR_RESPONSE && result.dispenser == DISPENSER_STATE_IDLING) {
    DispenseTestScreenParams params = {"Response Received"};
    draw_dispense_test_screen(phost, params);
    state = RECEIVED_RESPONSE;
  }

  return ControllerStepResult(result.steppers, result.dispenser);
}

int DispenseTestController::get_mode_type() const {
  return CONTROLLER_DISPENSE_TEST;
}
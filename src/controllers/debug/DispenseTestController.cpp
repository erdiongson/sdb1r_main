#include "DispenseTestController.h"
#include "../../views/debug/DispenseTestScreen.h"
#include "../../Constants.h"
#include "../../Utils.h"

DispenseTestController::DispenseTestController(ControllerParams params)
  : BaseController(params) {}

void DispenseTestController::onStart(Profile& profile) {
  Dprint(F("DispenseTestController::on_start"));
  
  DispenseTestScreenParams params = {"Ready"};
  drawDispenseTestScreen(phost, params);
}

void DispenseTestController::onInteraction(const Interaction& interaction) {
  int button = interaction.key_pressed;
  
  switch (button) {
    case TAG_DISPENSE_BACK:
      startNextController(CONTROLLER_DEBUG);
      break;
    
    case TAG_DISPENSE: {
      Dprint(F("DispenseTestController: Sending Dispense Command"));
      DispenseTestScreenParams params = {"SENT DISPENSE COMMAND"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.sendDispense();
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    // Vibration level buttons (U0-U4)
    case TAG_VIB_U0: {
      Dprint(F("DispenseTestController: Vibration level set to U0"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U0"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.setVibrationLevel(0);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_U1: {
      Dprint(F("DispenseTestController: Vibration level set to U1"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U1"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.setVibrationLevel(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_U2: {
      Dprint(F("DispenseTestController: Vibration level set to U2"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U2"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.setVibrationLevel(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_U3: {
      Dprint(F("DispenseTestController: Vibration level set to U3"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U3"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.setVibrationLevel(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_U4: {
      Dprint(F("DispenseTestController: Vibration level set to U4"));
      DispenseTestScreenParams params = {"SETTING VIB LEVEL U4"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.setVibrationLevel(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    // Vibration time buttons (1-5s)
    case TAG_VIB_TIME_1: {
      Dprint(F("DispenseTestController: Vibration time set to 1s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 1s"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.setVibrationTime(1);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_TIME_2: {
      Dprint(F("DispenseTestController: Vibration time set to 2s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 2s"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.setVibrationTime(2);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_TIME_3: {
      Dprint(F("DispenseTestController: Vibration time set to 3s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 3s"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.setVibrationTime(3);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_TIME_4: {
      Dprint(F("DispenseTestController: Vibration time set to 4s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 4s"};
      drawDispenseTestScreen(phost, params);
      dispenserHead.setVibrationTime(4);
      state = WAITING_FOR_RESPONSE;
      break;
    }
    
    case TAG_VIB_TIME_5: {
      Dprint(F("DispenseTestController: Vibration time set to 5s"));
      DispenseTestScreenParams params = {"SETTING VIB TIME 5s"};
      drawDispenseTestScreen(phost, params);
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

  if (state == WAITING_FOR_RESPONSE && result.dispenser == DISPENSER_STATE_IDLING) {
    DispenseTestScreenParams params = {"Response Received"};
    drawDispenseTestScreen(phost, params);
    state = RECEIVED_RESPONSE;
  }

  return ControllerStepResult(result.steppers, result.dispenser);
}

int DispenseTestController::getModeType() const {
  return CONTROLLER_DISPENSE_TEST;
}
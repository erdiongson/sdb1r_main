#include "DispenseTestController.h"
#include "../views/Dispense_Test_Screen.h"
#include "../Constants.h"

DispenseTestController::DispenseTestController(ControllerParams params)
  : BaseController(params) {}

void DispenseTestController::on_start(Profile& profile) {
  Dprint(F("DispenseTestController::on_start"));
  
  // Initialize UI
  DispenseTestSettings settings = {0, 0};
  Dispense_Test_Screen(phost, settings);
  
  // Initialize to home position
  dispenserHead.x().moveToMax();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMin();
}

void DispenseTestController::on_interaction(const Interaction& interaction) {
  int button = interaction.key_pressed;
  
  switch (button) {
    case TAG_DISPENSE_BACK:
      start_next_controller(CONTROLLER_HOME);
      break;
    
    case TAG_DISPENSE:
      Dprint(F("CONTROLLER: Dispense test - setting vibration and dispensing"));
      dispenserHead.send_dispense();
      break;
    
    // Vibration level buttons (U0-U4)
    case TAG_VIB_U0:
      Dprint(F("CONTROLLER: Vibration level set to U0"));
      break;
    
    case TAG_VIB_U1:
      Dprint(F("CONTROLLER: Vibration level set to U1"));
      break;
    
    case TAG_VIB_U2:
      Dprint(F("CONTROLLER: Vibration level set to U2"));
      break;
    
    case TAG_VIB_U3:
      Dprint(F("CONTROLLER: Vibration level set to U3"));
      break;
    
    case TAG_VIB_U4:
      Dprint(F("CONTROLLER: Vibration level set to U4"));
      break;
    
    // Vibration time buttons (1-5s)
    case TAG_VIB_TIME_1:
      Dprint(F("CONTROLLER: Vibration time set to 1s"));
      break;
    
    case TAG_VIB_TIME_2:
      Dprint(F("CONTROLLER: Vibration time set to 2s"));
      break;
    
    case TAG_VIB_TIME_3:
      Dprint(F("CONTROLLER: Vibration time set to 3s"));
      break;
    
    case TAG_VIB_TIME_4:
      Dprint(F("CONTROLLER: Vibration time set to 4s"));
      break;
    
    case TAG_VIB_TIME_5:
      Dprint(F("CONTROLLER: Vibration time set to 5s"));
      break;
    
    default:
      break;
  }
}

ControllerStepResult DispenseTestController::on_step() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int DispenseTestController::get_mode_type() const {
  return CONTROLLER_DISPENSE_TEST;
}
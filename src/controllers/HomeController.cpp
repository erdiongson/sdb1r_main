#include "HomeController.h"
#include "../views/Home_Screen.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../Utils.h"

HomeController::HomeController(ControllerParams params)
  : BaseController(params) {}

void HomeController::on_start(Profile& profile) {
  Dprint(F("HomeController::on_start"));
  Home_Screen(phost, MAINMENU);

  // Home the dispenser head
  dispenserHead.x().moveToMax();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMin();
}

void HomeController::on_interaction(const Interaction& interaction) {
  // Handle key presses
  switch (interaction.key_pressed) {
    case SETTING:
      Dprint(F("HomeController::on_interaction: Transitioning to settings"));
      start_next_controller(CONTROLLER_CONFIG);
      return;
    
    case START:
      Dprint(F("HomeController::on_interaction: Transitioning to start"));
      start_next_controller(CONTROLLER_RUN);
      return;
    
    default:
      break;
  }

  // Handle PLC messages
  switch (interaction.plc_message_type) {
    case MSG_START:
      Dprint(F("HomeController::on_interaction: Transitioning to start"));
      start_next_controller(CONTROLLER_RUN);
      break;
    
    case MSG_RAISE_Z:
      Dprint(F("HomeController::on_interaction: Raising Z"));
      dispenserHead.z().moveBy(-interaction.plc_message_data * STEPS_PER_UNIT_Z);
      break;
    
    case MSG_LOWER_Z:
      Dprint(F("HomeController::on_interaction: Lowering Z"));
      dispenserHead.z().moveBy(interaction.plc_message_data * STEPS_PER_UNIT_Z);
      break;
    
    default:
      break;
  }
}

ControllerStepResult HomeController::on_step() {
  DispenserProcessResult result = dispenserHead.process();
  return ControllerStepResult(result.steppers, result.dispenser);
}

int HomeController::get_mode_type() const {
  return CONTROLLER_HOME;
}
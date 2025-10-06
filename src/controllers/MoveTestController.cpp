#include "MoveTestController.h"
#include "../views/MoveTestScreen.h"
#include "../Constants.h"
#include "../../Config.h"
#include "../Utils.h"

MoveTestController::MoveTestController(ControllerParams params)
  : BaseController(params) {}

void MoveTestController::on_start(Profile& profile) {
  Dprint(F("MoveTestController::on_start"));
  Move_Test_Screen(phost, {});  
}

void MoveTestController::on_interaction(const Interaction& interaction) {
  int button = interaction.key_pressed;
  
  switch (button) {
    case TAG_MOVE_UP:
      Dprint(F("MoveTestController::on_interaction: Move up"));
      dispenserHead.y().moveBy(STEPS_PER_UNIT_Y * 1 * 30);
      break;
    
    case TAG_MOVE_DOWN:
      Dprint(F("MoveTestController::on_interaction: Move down"));
      dispenserHead.y().moveBy(-STEPS_PER_UNIT_Y * 1 * 30);
      break;
    
    case TAG_MOVE_LEFT:
      Dprint(F("MoveTestController::on_interaction: Move left"));
      dispenserHead.x().moveBy(STEPS_PER_UNIT_X * 1 * 30);
      break;
    
    case TAG_MOVE_RIGHT:
      Dprint(F("MoveTestController::on_interaction: Move right"));
      dispenserHead.x().moveBy(-STEPS_PER_UNIT_X * 1 * 30);
      break;
    
    case TAG_Z_UP:
      Dprint(F("MoveTestController::on_interaction: Move z up"));
      dispenserHead.z().moveBy(-STEPS_PER_UNIT_Z * 1 * 100);
      break;
    
    case TAG_Z_DOWN:
      Dprint(F("MoveTestController::on_interaction: Move z down"));
      dispenserHead.z().moveBy(STEPS_PER_UNIT_Z * 1 * 100);
      break;
    
    case TAG_MOVE_BACK:
      Dprint(F("MoveTestController::on_interaction: Move back"));
      start_next_controller(CONTROLLER_HOME);
      break;
    
    default:
      break;
  }
}

ControllerStepResult MoveTestController::on_step() {
  DispenserProcessResult result = dispenserHead.process();
  
  if (result.steppers == AXIS_STATE_COMPLETE) {
    LimitSwitchStates limitStates;
    limitStates.x_max_limit = dispenserHead.x().isAtMax();
    limitStates.x_min_limit = dispenserHead.x().isAtMin();
    limitStates.y_max_limit = dispenserHead.y().isAtMax();
    limitStates.y_min_limit = dispenserHead.y().isAtMin();
    limitStates.z_max_limit = dispenserHead.z().isAtMax();
    limitStates.z_min_limit = dispenserHead.z().isAtMin();
    Move_Test_Screen(phost, limitStates);
  }

  return ControllerStepResult(result.steppers, result.dispenser);
}

int MoveTestController::get_mode_type() const {
  return CONTROLLER_MOVE_TEST;
}

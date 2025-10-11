#include "MoveTestController.h"
#include "../../views/debug/MoveTestScreen.h"
#include "../../Constants.h"
#include "../../../Config.h"
#include "../../Utils.h"

MoveTestController::MoveTestController(ControllerParams params)
  : BaseController(params) {}

void MoveTestController::onStart() {
  Logger::log(F("MoveTestController::on_start"));
  drawMoveTestScreen(phost, {});  
}

void MoveTestController::onInteraction(const Interaction& interaction) {
  int button = interaction.key_pressed;
  
  switch (button) {
    case TAG_MOVE_UP:
      Logger::log(F("MoveTestController::on_interaction: Move up"));
      dispenserHead.y().moveBy(STEPS_PER_UNIT_Y * 1 * 50);
      break;
    
    case TAG_MOVE_DOWN:
      Logger::log(F("MoveTestController::on_interaction: Move down"));
      dispenserHead.y().moveBy(-STEPS_PER_UNIT_Y * 1 * 50);
      break;
    
    case TAG_MOVE_LEFT:
      Logger::log(F("MoveTestController::on_interaction: Move left"));
      dispenserHead.x().moveBy(STEPS_PER_UNIT_X * 1 * 50);
      break;
    
    case TAG_MOVE_RIGHT:
      Logger::log(F("MoveTestController::on_interaction: Move right"));
      dispenserHead.x().moveBy(-STEPS_PER_UNIT_X * 1 * 50);
      break;
    
    case TAG_Z_UP:
      Logger::log(F("MoveTestController::on_interaction: Move z up"));
      dispenserHead.z().moveBy(-STEPS_PER_UNIT_Z * 1 * 30);
      break;
    
    case TAG_Z_DOWN:
      Logger::log(F("MoveTestController::on_interaction: Move z down"));
      dispenserHead.z().moveBy(STEPS_PER_UNIT_Z * 1 * 30);
      break;
    
    case TAG_MOVE_BACK:
      Logger::log(F("MoveTestController::on_interaction: Move back"));
      startNextController(CONTROLLER_DEBUG);
      break;
    
    default:
      break;
  }
}

ControllerStepResult MoveTestController::onStep() {
  DispenserProcessResult result = dispenserHead.process();
  
  if (result.steppers == AXIS_STATE_COMPLETE) {
    LimitSwitchStates limitStates;
    limitStates.x_max_limit = dispenserHead.x().isAtMax();
    limitStates.x_min_limit = dispenserHead.x().isAtMin();
    limitStates.y_max_limit = dispenserHead.y().isAtMax();
    limitStates.y_min_limit = dispenserHead.y().isAtMin();
    limitStates.z_max_limit = dispenserHead.z().isAtMax();
    limitStates.z_min_limit = dispenserHead.z().isAtMin();
    drawMoveTestScreen(phost, limitStates);
  }

  return ControllerStepResult(result.steppers, result.dispenser);
}

int MoveTestController::getModeType() const {
  return CONTROLLER_MOVE_TEST;
}

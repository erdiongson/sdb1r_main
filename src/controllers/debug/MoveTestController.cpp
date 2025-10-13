#include "MoveTestController.h"
#include "../../views/debug/MoveTestScreen.h"
#include "../../views/common/Keyboards.h"

MoveTestController::MoveTestController(ControllerParams params) : BaseController(params) {}

void MoveTestController::onStart() {
  Logger::log(F("MoveTestController::on_start"));

  LimitSwitchStates limitStates = {};
  limitStates.x_max_limit = false;
  limitStates.x_min_limit = false;
  limitStates.y_max_limit = false;
  limitStates.y_min_limit = false;
  limitStates.z_max_limit = false;
  limitStates.z_min_limit = false;

  MoveTestParams params;
  params.xy_distance_cm = xy_distance_cm;
  params.z_distance_cm = z_distance_cm;
  params.bounce_count = bounce_count;

  drawMoveTestScreen(phost, limitStates, params);
}

void MoveTestController::onInteraction(const Interaction& interaction) {
  int button = interaction.key_pressed;

  switch (button) {
    case TAG_MOVE_UP:
      Logger::log(F("MoveTestController::on_interaction: Move up"));
      dispenserHead.y().moveBy(STEPS_PER_UNIT_Y * xy_distance_cm * CM_TO_MM_MULTIPLIER);
      break;

    case TAG_MOVE_DOWN:
      Logger::log(F("MoveTestController::on_interaction: Move down"));
      dispenserHead.y().moveBy(-STEPS_PER_UNIT_Y * xy_distance_cm * CM_TO_MM_MULTIPLIER);
      break;

    case TAG_MOVE_LEFT:
      Logger::log(F("MoveTestController::on_interaction: Move left"));
      dispenserHead.x().moveBy(STEPS_PER_UNIT_X * xy_distance_cm * CM_TO_MM_MULTIPLIER);
      break;

    case TAG_MOVE_RIGHT:
      Logger::log(F("MoveTestController::on_interaction: Move right"));
      dispenserHead.x().moveBy(-STEPS_PER_UNIT_X * xy_distance_cm * CM_TO_MM_MULTIPLIER);
      break;

    case TAG_Z_UP:
      Logger::log(F("MoveTestController::on_interaction: Move z up"));
      dispenserHead.z().moveBy(-STEPS_PER_UNIT_Z * z_distance_cm * CM_TO_MM_MULTIPLIER);
      break;

    case TAG_Z_DOWN:
      Logger::log(F("MoveTestController::on_interaction: Move z down"));
      dispenserHead.z().moveBy(STEPS_PER_UNIT_Z * z_distance_cm * CM_TO_MM_MULTIPLIER);
      break;

    case TAG_MOVE_XY_DIST:
      Logger::log(F("MoveTestController::on_interaction: Set XY distance"));
      xy_distance_cm = getKeypadValue(phost, xy_distance_cm, MIN_MOVE_DISTANCE_CM, MAX_MOVE_DISTANCE_CM, true);
      {
        MoveTestParams params;
        params.xy_distance_cm = xy_distance_cm;
        params.z_distance_cm = z_distance_cm;
        params.bounce_count = bounce_count;
        drawMoveTestScreen(phost, {}, params);
      }
      break;

    case TAG_MOVE_Z_DIST:
      Logger::log(F("MoveTestController::on_interaction: Set Z distance"));
      z_distance_cm = getKeypadValue(phost, z_distance_cm, MIN_MOVE_DISTANCE_CM, MAX_MOVE_DISTANCE_CM, true);
      {
        MoveTestParams params;
        params.xy_distance_cm = xy_distance_cm;
        params.z_distance_cm = z_distance_cm;
        params.bounce_count = bounce_count;
        drawMoveTestScreen(phost, {}, params);
      }
      break;

    case TAG_MOVE_BOUNCE:
      Logger::log(F("MoveTestController::on_interaction: Set bounce count"));
      bounce_count = (int)getKeypadValue(phost, bounce_count, MIN_MOVE_DISTANCE_CM, MAX_BOUNCE_COUNT, false);
      {
        MoveTestParams params;
        params.xy_distance_cm = xy_distance_cm;
        params.z_distance_cm = z_distance_cm;
        params.bounce_count = bounce_count;
        drawMoveTestScreen(phost, {}, params);
      }
      break;

    case TAG_MOVE_STOP:
      Logger::log(F("MoveTestController::on_interaction: Stop all movement"));
      dispenserHead.x().stop();
      dispenserHead.y().stop();
      dispenserHead.z().stop();
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

    MoveTestParams params;
    params.xy_distance_cm = xy_distance_cm;
    params.z_distance_cm = z_distance_cm;
    params.bounce_count = bounce_count;
    drawMoveTestScreen(phost, limitStates, params);
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

int MoveTestController::getModeType() const {
  return CONTROLLER_MOVE_TEST;
}

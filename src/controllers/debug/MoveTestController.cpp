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
  params.current_bounce_count = 0;
  params.total_bounce_count = 0;
  params.blocking = blocking;

  drawMoveTestScreen(phost, limitStates, params);
}

void MoveTestController::onInteraction(const Interaction& interaction) {
  int button = interaction.key_pressed;

  switch (button) {
    case TAG_MOVE_UP:
      Logger::log(F("MoveTestController::on_interaction: Move up"));
      bounce_state.current_count = bounce_count;
      bounce_state.total_count = bounce_count;
      bounce_state.current_stage = BOUNCE_STAGE_GO;
      bounce_state.axis = BOUNCE_AXIS_Y;
      bounce_state.go_amount = STEPS_PER_UNIT_Y * xy_distance_cm * CM_TO_MM_MULTIPLIER;
      dispenserHead.y().moveBy(bounce_state.go_amount);
      if (blocking) dispenserHead.y().runUntilCompleteBlocking();
      break;

    case TAG_MOVE_DOWN:
      Logger::log(F("MoveTestController::on_interaction: Move down"));
      bounce_state.current_count = bounce_count;
      bounce_state.total_count = bounce_count;
      bounce_state.current_stage = BOUNCE_STAGE_GO;
      bounce_state.axis = BOUNCE_AXIS_Y;
      bounce_state.go_amount = -STEPS_PER_UNIT_Y * xy_distance_cm * CM_TO_MM_MULTIPLIER;
      dispenserHead.y().moveBy(bounce_state.go_amount);
      if (blocking) dispenserHead.y().runUntilCompleteBlocking();
      break;

    case TAG_MOVE_LEFT:
      Logger::log(F("MoveTestController::on_interaction: Move left"));
      bounce_state.current_count = bounce_count;
      bounce_state.total_count = bounce_count;
      bounce_state.current_stage = BOUNCE_STAGE_GO;
      bounce_state.axis = BOUNCE_AXIS_X;
      bounce_state.go_amount = STEPS_PER_UNIT_X * xy_distance_cm * CM_TO_MM_MULTIPLIER;
      dispenserHead.x().moveBy(bounce_state.go_amount);
      if (blocking) dispenserHead.x().runUntilCompleteBlocking();
      break;

    case TAG_MOVE_RIGHT:
      Logger::log(F("MoveTestController::on_interaction: Move right"));
      bounce_state.current_count = bounce_count;
      bounce_state.total_count = bounce_count;
      bounce_state.current_stage = BOUNCE_STAGE_GO;
      bounce_state.axis = BOUNCE_AXIS_X;
      bounce_state.go_amount = -STEPS_PER_UNIT_X * xy_distance_cm * CM_TO_MM_MULTIPLIER;
      dispenserHead.x().moveBy(bounce_state.go_amount);
      if (blocking) dispenserHead.x().runUntilCompleteBlocking();
      break;

    case TAG_Z_UP:
      Logger::log(F("MoveTestController::on_interaction: Move z up"));
      bounce_state.current_count = bounce_count;
      bounce_state.total_count = bounce_count;
      bounce_state.current_stage = BOUNCE_STAGE_GO;
      bounce_state.axis = BOUNCE_AXIS_Z;
      bounce_state.go_amount = STEPS_PER_UNIT_Z * z_distance_cm * CM_TO_MM_MULTIPLIER;
      dispenserHead.z().moveBy(bounce_state.go_amount);
      if (blocking) dispenserHead.z().runUntilCompleteBlocking();
      break;

    case TAG_Z_DOWN:
      Logger::log(F("MoveTestController::on_interaction: Move z down"));
      bounce_state.current_count = bounce_count;
      bounce_state.total_count = bounce_count;
      bounce_state.current_stage = BOUNCE_STAGE_GO;
      bounce_state.axis = BOUNCE_AXIS_Z;
      bounce_state.go_amount = -STEPS_PER_UNIT_Z * z_distance_cm * CM_TO_MM_MULTIPLIER;
      dispenserHead.z().moveBy(bounce_state.go_amount);
      if (blocking) dispenserHead.z().runUntilCompleteBlocking();
      break;

    case TAG_MOVE_XY_DIST:
      Logger::log(F("MoveTestController::on_interaction: Set XY distance"));
      xy_distance_cm = getKeypadValue(phost, xy_distance_cm, MIN_MOVE_DISTANCE_CM, MAX_MOVE_DISTANCE_CM, true);
      {
        MoveTestParams params;
        params.xy_distance_cm = xy_distance_cm;
        params.z_distance_cm = z_distance_cm;
        params.bounce_count = bounce_count;
        params.current_bounce_count = 0;
        params.total_bounce_count = 0;
        params.blocking = blocking;
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
        params.current_bounce_count = 0;
        params.total_bounce_count = 0;
        params.blocking = blocking;
        drawMoveTestScreen(phost, {}, params);
      }
      break;

    case TAG_MOVE_BOUNCE:
      Logger::log(F("MoveTestController::on_interaction: Set bounce count"));
      bounce_count = (int)getKeypadValue(phost, bounce_count, MIN_BOUNCE_COUNT, MAX_BOUNCE_COUNT, false);
      {
        MoveTestParams params;
        params.xy_distance_cm = xy_distance_cm;
        params.z_distance_cm = z_distance_cm;
        params.bounce_count = bounce_count;
        params.current_bounce_count = 0;
        params.total_bounce_count = 0;
        params.blocking = blocking;
        drawMoveTestScreen(phost, {}, params);
      }
      break;

    case TAG_MOVE_STOP:
      Logger::log(F("MoveTestController::on_interaction: Stop all movement"));
      dispenserHead.x().stopRunning();
      dispenserHead.y().stopRunning();
      dispenserHead.z().stopRunning();
      bounce_state.current_count = 0;
      bounce_state.total_count = 0;
      updateScreen();
      
      break;

    case TAG_MOVE_BLOCKING:
      Logger::log(F("MoveTestController::on_interaction: Toggle blocking"));
      blocking = !blocking;
      updateScreen();
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

    // Bounce
    if (bounce_state.current_count > 0) {
      if (bounce_state.current_stage == BOUNCE_STAGE_GO) {
        // Switch to RETURN stage and move back
        bounce_state.current_stage = BOUNCE_STAGE_RETURN;
        if (bounce_state.axis == BOUNCE_AXIS_X) {
          dispenserHead.x().moveBy(-bounce_state.go_amount);
        } else if (bounce_state.axis == BOUNCE_AXIS_Y) {
          dispenserHead.y().moveBy(-bounce_state.go_amount);
        } else if (bounce_state.axis == BOUNCE_AXIS_Z) {
          dispenserHead.z().moveBy(-bounce_state.go_amount);
        }
      } else if (bounce_state.current_stage == BOUNCE_STAGE_RETURN) {
        // Decrement count
        bounce_state.current_count--;
        if (bounce_state.current_count > 0) {
          // Switch to GO stage and move forward again
          bounce_state.current_stage = BOUNCE_STAGE_GO;
          if (bounce_state.axis == BOUNCE_AXIS_X) {
            dispenserHead.x().moveBy(bounce_state.go_amount);
          } else if (bounce_state.axis == BOUNCE_AXIS_Y) {
            dispenserHead.y().moveBy(bounce_state.go_amount);
          } else if (bounce_state.axis == BOUNCE_AXIS_Z) {
            dispenserHead.z().moveBy(bounce_state.go_amount);
          }
        }
      }
    }
  }

  // Update screen at regular intervals regardless of stepper state
  static unsigned long last_update_time = 0;
  unsigned long current_time = millis();
  if (current_time - last_update_time >= MOVE_TEST_REFRESH_INTERVAL_MS) {
    last_update_time = current_time;
    updateScreen();
  }

  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

void MoveTestController::updateScreen() {
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
  // Calculate current bounce number (total - remaining + 1)
  params.current_bounce_count = bounce_state.total_count > 0 ? (bounce_state.total_count - bounce_state.current_count + 1) : 0;
  params.total_bounce_count = bounce_state.total_count;
  params.blocking = blocking;
  drawMoveTestScreen(phost, limitStates, params);
}

int MoveTestController::getModeType() const {
  return CONTROLLER_MOVE_TEST;
}

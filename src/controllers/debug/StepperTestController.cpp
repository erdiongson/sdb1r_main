#include "StepperTestController.h"
#include "../../views/debug/StepperTestScreen.h"
#include "../../views/common/Keyboards.h"
#include "../../views/ViewCommon.h"

StepperTestController::StepperTestController(ControllerParams params) : BaseController(params) {}

void StepperTestController::onStart() {
  Logger::log(F("StepperTestController::onStart"));

  // Load initial axis parameters
  loadAxisParameters();

  // Initialize status message and tracking state
  g_message_buffer[0] = '\0';
  is_tracking_movement = false;
  step_count = 0;

  updateScreen();
}

void StepperTestController::onInteraction(const Interaction& interaction) {
  int button = interaction.key_pressed;

  switch (button) {
    case TAG_STEPPER_AXIS_X:
      Logger::log(F("StepperTestController::onInteraction: Select X axis"));
      selected_axis = STEPPER_AXIS_X;
      loadAxisParameters();
      updateScreen();
      break;

    case TAG_STEPPER_AXIS_Y:
      Logger::log(F("StepperTestController::onInteraction: Select Y axis"));
      selected_axis = STEPPER_AXIS_Y;
      loadAxisParameters();
      updateScreen();
      break;

    case TAG_STEPPER_AXIS_Z:
      Logger::log(F("StepperTestController::onInteraction: Select Z axis"));
      selected_axis = STEPPER_AXIS_Z;
      loadAxisParameters();
      updateScreen();
      break;

    case TAG_STEPPER_SPEED_VALUE:
      Logger::log(F("StepperTestController::onInteraction: Set speed value"));
      current_max_speed = getKeypadValue(phost, current_max_speed, 0, 100000, false);
      getSelectedAxis().setMaxSpeed(current_max_speed);
      updateScreen();
      break;

    case TAG_STEPPER_SPEED_MINUS_10K:
      Logger::log(F("StepperTestController::onInteraction: Speed -10000"));
      current_max_speed -= 10000;
      if (current_max_speed < 0) current_max_speed = 0;
      getSelectedAxis().setMaxSpeed(current_max_speed);
      updateScreen();
      break;

    case TAG_STEPPER_SPEED_PLUS_10K:
      Logger::log(F("StepperTestController::onInteraction: Speed +10000"));
      current_max_speed += 10000;
      getSelectedAxis().setMaxSpeed(current_max_speed);
      updateScreen();
      break;

    case TAG_STEPPER_SPEED_MINUS_1K:
      Logger::log(F("StepperTestController::onInteraction: Speed -1000"));
      current_max_speed -= 1000;
      if (current_max_speed < 0) current_max_speed = 0;
      getSelectedAxis().setMaxSpeed(current_max_speed);
      updateScreen();
      break;

    case TAG_STEPPER_SPEED_MINUS_100:
      Logger::log(F("StepperTestController::onInteraction: Speed -100"));
      current_max_speed -= 100;
      if (current_max_speed < 0) current_max_speed = 0;
      getSelectedAxis().setMaxSpeed(current_max_speed);
      updateScreen();
      break;

    case TAG_STEPPER_SPEED_PLUS_100:
      Logger::log(F("StepperTestController::onInteraction: Speed +100"));
      current_max_speed += 100;
      getSelectedAxis().setMaxSpeed(current_max_speed);
      updateScreen();
      break;

    case TAG_STEPPER_SPEED_PLUS_1K:
      Logger::log(F("StepperTestController::onInteraction: Speed +1000"));
      current_max_speed += 1000;
      getSelectedAxis().setMaxSpeed(current_max_speed);
      updateScreen();
      break;

    case TAG_STEPPER_SPEED_RESET:
      Logger::log(F("StepperTestController::onInteraction: Reset speed"));
      current_max_speed = original_max_speed;
      getSelectedAxis().setMaxSpeed(current_max_speed);
      updateScreen();
      break;

    case TAG_STEPPER_ACCEL_VALUE:
      Logger::log(F("StepperTestController::onInteraction: Set acceleration value"));
      current_max_acceleration = getKeypadValue(phost, current_max_acceleration, 0, 1000000, false);
      getSelectedAxis().setMaxAcceleration(current_max_acceleration);
      updateScreen();
      break;

    case TAG_STEPPER_ACCEL_MINUS_10K:
      Logger::log(F("StepperTestController::onInteraction: Accel -10000"));
      current_max_acceleration -= 10000;
      if (current_max_acceleration < 0) current_max_acceleration = 0;
      getSelectedAxis().setMaxAcceleration(current_max_acceleration);
      updateScreen();
      break;

    case TAG_STEPPER_ACCEL_PLUS_10K:
      Logger::log(F("StepperTestController::onInteraction: Accel +10000"));
      current_max_acceleration += 10000;
      getSelectedAxis().setMaxAcceleration(current_max_acceleration);
      updateScreen();
      break;

    case TAG_STEPPER_ACCEL_MINUS_1K:
      Logger::log(F("StepperTestController::onInteraction: Accel -1000"));
      current_max_acceleration -= 1000;
      if (current_max_acceleration < 0) current_max_acceleration = 0;
      getSelectedAxis().setMaxAcceleration(current_max_acceleration);
      updateScreen();
      break;

    case TAG_STEPPER_ACCEL_MINUS_100:
      Logger::log(F("StepperTestController::onInteraction: Accel -100"));
      current_max_acceleration -= 100;
      if (current_max_acceleration < 0) current_max_acceleration = 0;
      getSelectedAxis().setMaxAcceleration(current_max_acceleration);
      updateScreen();
      break;

    case TAG_STEPPER_ACCEL_PLUS_100:
      Logger::log(F("StepperTestController::onInteraction: Accel +100"));
      current_max_acceleration += 100;
      getSelectedAxis().setMaxAcceleration(current_max_acceleration);
      updateScreen();
      break;

    case TAG_STEPPER_ACCEL_PLUS_1K:
      Logger::log(F("StepperTestController::onInteraction: Accel +1000"));
      current_max_acceleration += 1000;
      getSelectedAxis().setMaxAcceleration(current_max_acceleration);
      updateScreen();
      break;

    case TAG_STEPPER_ACCEL_RESET:
      Logger::log(F("StepperTestController::onInteraction: Reset acceleration"));
      current_max_acceleration = original_max_acceleration;
      getSelectedAxis().setMaxAcceleration(current_max_acceleration);
      updateScreen();
      break;

    case TAG_STEPPER_MOVE_MINUS:
      Logger::log(F("StepperTestController::onInteraction: Move minus"));
      {
        Axis& axis = getSelectedAxis();
        long steps = 0;
        if (selected_axis == STEPPER_AXIS_X) {
          steps = -STEPS_PER_UNIT_X * move_amount_cm * CM_TO_MM_MULTIPLIER;
        } else if (selected_axis == STEPPER_AXIS_Y) {
          steps = -STEPS_PER_UNIT_Y * move_amount_cm * CM_TO_MM_MULTIPLIER;
        } else if (selected_axis == STEPPER_AXIS_Z) {
          steps = -STEPS_PER_UNIT_Z * move_amount_cm * CM_TO_MM_MULTIPLIER;
        }
        axis.moveBy(steps);
        move_start_time = millis();
        step_count = 0;
        if (blocking) {
          step_count = axis.runUntilCompleteBlocking();
          unsigned long elapsed_ms = millis() - move_start_time;
          if (elapsed_ms > 0) {
            float steps_per_sec = (step_count * 1000.0f) / elapsed_ms;
            long rate_int = (long)steps_per_sec;
            long rate_dec = (long)((steps_per_sec - rate_int) * 10);
            snprintf(g_message_buffer, sizeof(g_message_buffer), "Steps: %lu, Time: %lums, Rate: %ld.%ld steps/s", step_count, elapsed_ms, rate_int, rate_dec);
          } else {
            snprintf(g_message_buffer, sizeof(g_message_buffer), "Steps: %lu, Time: %lums", step_count, elapsed_ms);
          }
          updateScreen();
        } else {
          is_tracking_movement = true;
        }
      }
      break;

    case TAG_STEPPER_MOVE_PLUS:
      Logger::log(F("StepperTestController::onInteraction: Move plus"));
      {
        Axis& axis = getSelectedAxis();
        long steps = 0;
        if (selected_axis == STEPPER_AXIS_X) {
          steps = STEPS_PER_UNIT_X * move_amount_cm * CM_TO_MM_MULTIPLIER;
        } else if (selected_axis == STEPPER_AXIS_Y) {
          steps = STEPS_PER_UNIT_Y * move_amount_cm * CM_TO_MM_MULTIPLIER;
        } else if (selected_axis == STEPPER_AXIS_Z) {
          steps = STEPS_PER_UNIT_Z * move_amount_cm * CM_TO_MM_MULTIPLIER;
        }
        axis.moveBy(steps);
        move_start_time = millis();
        step_count = 0;
        if (blocking) {
          step_count = axis.runUntilCompleteBlocking();
          unsigned long elapsed_ms = millis() - move_start_time;
          if (elapsed_ms > 0) {
            float steps_per_sec = (step_count * 1000.0f) / elapsed_ms;
            long rate_int = (long)steps_per_sec;
            long rate_dec = (long)((steps_per_sec - rate_int) * 10);
            snprintf(g_message_buffer, sizeof(g_message_buffer), "Steps: %lu, Time: %lums, Rate: %ld.%ld steps/s", step_count, elapsed_ms, rate_int, rate_dec);
          } else {
            snprintf(g_message_buffer, sizeof(g_message_buffer), "Steps: %lu, Time: %lums", step_count, elapsed_ms);
          }
          updateScreen();
        } else {
          is_tracking_movement = true;
        }
      }
      break;

    case TAG_STEPPER_MOVE_AMOUNT:
      Logger::log(F("StepperTestController::onInteraction: Set move amount"));
      move_amount_cm = getKeypadValue(phost, move_amount_cm, MIN_MOVE_DISTANCE_CM, MAX_MOVE_DISTANCE_CM, true);
      updateScreen();
      break;

    case TAG_STEPPER_BLOCKING:
      Logger::log(F("StepperTestController::onInteraction: Toggle blocking"));
      blocking = !blocking;
      updateScreen();
      break;

    case TAG_STEPPER_BACK:
      Logger::log(F("StepperTestController::onInteraction: Back"));
      startNextController(CONTROLLER_DEBUG);
      break;

    case TAG_STEPPER_STOP:
      Logger::log(F("StepperTestController::onInteraction: Stop all axes"));
      dispenserHead.x().stopRunning();
      dispenserHead.y().stopRunning();
      dispenserHead.z().stopRunning();
      is_tracking_movement = false;
      strncpy(g_message_buffer, PROGMEM_STR(F("All axes stopped")), sizeof(g_message_buffer));
      updateScreen();
      break;

    default:
      break;
  }
}

ControllerStepResult StepperTestController::onStep() {
  DispenserProcessResult result = dispenserHead.process();
  
  // Track non-blocking movement
  if (is_tracking_movement) {
    if (result.steppers == AXIS_STATE_RUNNING) {
      // Movement is still running, increment step count
      step_count++;
    } else {
      // Movement completed, calculate and display statistics
      unsigned long elapsed_ms = millis() - move_start_time;
      if (elapsed_ms > 0) {
        float steps_per_sec = (step_count * 1000.0f) / elapsed_ms;
        long rate_int = (long)steps_per_sec;
        long rate_dec = (long)((steps_per_sec - rate_int) * 10);
        snprintf_P(g_message_buffer, sizeof(g_message_buffer), PSTR("Steps: %lu, Time: %lums, Rate: %ld.%ld steps/s"), step_count, elapsed_ms, rate_int, rate_dec);
      } else {
        snprintf_P(g_message_buffer, sizeof(g_message_buffer), PSTR("Steps: %lu, Time: %lums"), step_count, elapsed_ms);
      }
      is_tracking_movement = false;
      updateScreen();
    }
  }
  
  return ControllerStepResult(result.steppers == AXIS_STATE_RUNNING);
}

void StepperTestController::updateScreen() {
  StepperTestParams params;
  params.selected_axis = selected_axis;
  params.max_speed = current_max_speed;
  params.max_acceleration = current_max_acceleration;
  params.move_amount_cm = move_amount_cm;
  params.blocking = blocking;
  params.status_message = g_message_buffer;

  drawStepperTestScreen(phost, params);
}

void StepperTestController::loadAxisParameters() {
  Axis& axis = getSelectedAxis();
  current_max_speed = axis.getMaxSpeed();
  current_max_acceleration = axis.getMaxAcceleration();
  original_max_speed = current_max_speed;
  original_max_acceleration = current_max_acceleration;
}

Axis& StepperTestController::getSelectedAxis() {
  switch (selected_axis) {
    case STEPPER_AXIS_X:
      return dispenserHead.x();
    case STEPPER_AXIS_Y:
      return dispenserHead.y();
    case STEPPER_AXIS_Z:
      return dispenserHead.z();
    default:
      return dispenserHead.x();
  }
}

int StepperTestController::getModeType() const {
  return CONTROLLER_STEPPER_TEST;
}

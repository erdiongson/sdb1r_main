#include "RunController.h"
#include "../views/MainScreen.h"
#include "../views/common/Dialogs.h"
#include "../Utils.h"
#include "../serial/PlcSerial.h"

RunController::RunController(ControllerParams params) : BaseController(params) {}

void RunController::onStart() {
  this->profile = profile_manager.getCurrentProfile();
  trayHandler.loadProfile(profile_manager.getCurrentProfile());
  TrayHandler::Position first_position = trayHandler.reset();

  if (first_position.x == -1 || first_position.y == -1) {
    Logger::log(F("MODE: No valid positions found"));
    MainScreenParams params = { profile, { 0, 0, 0, 0 }, DIALOG_ERROR_NO_VALID_POSITIONS };
    drawRunScreen(params);
    pause();
    return;
  }

  // Reset dispenser state and clear any incoming messages
  dispenserHead.resetDispenser();

  // Set the current position as the default 0 as a safe position
  // in the case when STOP is triggered before the Zeroing is completed
  dispenserHead.z().reset();

  // Set busy state
  PlcSerial::setBusy(true);

  // Actually start the sequence
  start();
}

// Pauses the run by stopping all axis movements immediately.
void RunController::pause() {
  Logger::log(F("MODE: Paused"));
  paused = true;
  dispenserHead.x().stop();
  dispenserHead.y().stop();
  dispenserHead.z().stop();

  // Clear busy state
  PlcSerial::setBusy(false);
}

// Resumes the run from a paused state.
void RunController::resume() {
  Logger::log(F("MODE: Resumed"));
  PlcSerial::setBusy(true);
  paused = false;
  startStage(stage);
  drawRunScreen({ profile, getRunStatus(), 0 });
}

// Stops the run and returns to home position.
void RunController::stop() {
  Logger::log(F("MODE: Stopped"));
  drawStoppingScreen({ profile, { 0, 0, 0, 0 }, 0 });
  paused = false;

  // Stop all stepper movements
  dispenserHead.x().stopRunning();
  dispenserHead.y().stopRunning();
  dispenserHead.z().stopRunning();

  // Wait until dispenser is finished with any ongoing action
  while (dispenserHead.getState() != DISPENSER_STATE_IDLING) {
    dispenserHead.process();
  }

  startNextController(CONTROLLER_HOMING);
}

// Starts a new stage of the run
// The funcitonality must be idempotent, since it may be called again after a "pause"
void RunController::startStage(Stage new_stage) {
  switch (new_stage) {
    case STAGE_IDLE:
      dispenserHead.x().stopRunning();
      dispenserHead.y().stopRunning();
      dispenserHead.z().stopRunning();
      break;

    case STAGE_SET_VIB_LEVEL:
      Logger::log(F("STAGE: Setting vibration level"));
      this->stage = STAGE_SET_VIB_LEVEL;
      dispenserHead.setVibrationLevel(profile.vibration_level);
      break;

    case STAGE_SET_VIB_DURATION:
      Logger::log(F("STAGE: Setting vibration duration to "), (uint8_t)profile.vibration_duration);
      this->stage = STAGE_SET_VIB_DURATION;
      dispenserHead.setVibrationTime(profile.vibration_duration);
      break;

    case STAGE_ZERO:
      Logger::log(F("STAGE: Moving to zero position"));
      this->stage = STAGE_ZERO;
      dispenserHead.x().moveToMax();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMax();
      break;

    case STAGE_START_PRIME:
      Logger::log(F("STAGE: Starting prime"));
      this->stage = STAGE_START_PRIME;
      dispenserHead.sendDispense();

      // Immediately move to wait stage
      startStage(STAGE_WAIT_PRIME);
      break;

    case STAGE_WAIT_PRIME:
      Logger::log(F("STAGE: Waiting for prime to complete"));
      this->stage = STAGE_WAIT_PRIME;
      break;

    case STAGE_MOVE:
      snprintf(g_log_buffer, sizeof(g_log_buffer), "STAGE: Moving to position X=%ld, Y=%ld", target_x, target_y);
      Logger::log(g_log_buffer);
      this->stage = STAGE_MOVE;
      dispenserHead.x().moveTo(target_x);
      dispenserHead.y().moveTo(target_y);
      break;

    case STAGE_LOWER_HEAD:
      snprintf(g_log_buffer, sizeof(g_log_buffer), "STAGE: Lowering head to Z=%ld", (long)(STEPS_PER_UNIT_Z * profile.z_dip));
      Logger::log(g_log_buffer);
      this->stage = STAGE_LOWER_HEAD;
      dispenserHead.z().moveTo(-STEPS_PER_UNIT_Z * profile.z_dip);
      break;

    case STAGE_START_DISPENSE:
      Logger::log(F("STAGE: Starting dispensing"));
      this->stage = STAGE_START_DISPENSE;
      dispenserHead.sendDispense();

      // Immediately move to wait stage
      startStage(STAGE_WAIT_DISPENSE);
      break;

    case STAGE_WAIT_DISPENSE:
      Logger::log(F("STAGE: Waiting for dispensing to complete"));
      this->stage = STAGE_WAIT_DISPENSE;
      break;

    case STAGE_RAISE_HEAD:
      Logger::log(F("STAGE: Raising head"));
      this->stage = STAGE_RAISE_HEAD;
      dispenserHead.z().moveToMax();
      break;

    default:
      break;
  }
}

ControllerStepResult RunController::onStep() {
  if (paused) return ControllerStepResult(false);

  DispenserProcessResult dispenser_process_result = dispenserHead.process();
  if (dispenser_process_result.steppers == AXIS_STATE_RUNNING) {
    return ControllerStepResult(true);
  }

  MainScreenParams params = { profile, getRunStatus(), 0 };

  if (dispenser_process_result.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    Logger::log(F("MODE: Stepper error - limit switch triggered"));
    params.dialog_code = DIALOG_ERROR_LIMIT_SWITCH;
    drawRunScreen(params);
    pause();
    return ControllerStepResult(false);
  }

  if (dispenser_process_result.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    Logger::log(F("MODE: Dispenser error - IR sensor failure"));
    params.dialog_code = DIALOG_ERROR_IR_SENSOR;
    drawRunScreen(params);
    pause();
    return ControllerStepResult(false);
  }

  if (dispenser_process_result.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    Logger::log(F("MODE: Dispenser error - Acknowledgment error"));
    params.dialog_code = DIALOG_ERROR_ACK_ERROR;
    drawRunScreen(params);
    pause();
    return ControllerStepResult(false);
  }

  if (dispenser_process_result.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    Logger::log(F("MODE: Dispenser error - marker not detected"));
    params.dialog_code = DIALOG_ERROR_MARKER_NOT_DETECTED;
    drawRunScreen(params);
    pause();

    return ControllerStepResult(false);
  }

  if (dispenser_process_result.dispenser == DISPENSER_STATE_ERROR_CYCLES_TIMEOUT) {
    Logger::log(F("MODE: Dispenser error - cycle timeout"));
    params.dialog_code = DIALOG_ERROR_CYCLE_TIMEOUT;
    drawRunScreen(params);
    pause();

    return ControllerStepResult(false);
  }

  // Perform logic after all axis are idle (all movement is completed)
  processStageLogic(dispenser_process_result);

  return ControllerStepResult(false);
}

int RunController::getModeType() const {
  return CONTROLLER_RUN;
}

// Starts the run sequence
void RunController::start() {
  Logger::log(F("MODE: Starting run"));
  paused = false;
  cycle = 0;
  drawRunScreen({ profile, { 0, 0, 0, 0 }, 0 });
  startStage(STAGE_SET_VIB_LEVEL);
}

// Handles user interactions during run mode
void RunController::onInteraction(const Interaction& interaction) {
  if (interaction.plc_message_type == MSG_STOP || interaction.key_pressed == TAG_STOP) {
    stop();
    return;
  }

  if (interaction.plc_message_type == MSG_PAUSE || interaction.key_pressed == TAG_PAUSE) {
    pause();
    drawPauseScreen({ profile, getRunStatus(), 0 });
    return;
  }

  if (interaction.plc_message_type == MSG_START || interaction.key_pressed == TAG_START) {
    resume();
    return;
  }
}

// Processes the stage logic based on current stage and dispenser state
void RunController::processStageLogic(DispenserProcessResult& dispenser_process_result) {
  switch (stage) {
    case STAGE_SET_VIB_LEVEL:
      if (dispenser_process_result.dispenser != DISPENSER_STATE_IDLING) break;
      Logger::log(F("Vibration Level set, setting Vibration Duration"));
      startStage(STAGE_SET_VIB_DURATION);
      break;

    case STAGE_SET_VIB_DURATION:
      if (dispenser_process_result.dispenser != DISPENSER_STATE_IDLING) break;
      Logger::log(F("Vibration Duration set, moving to zero position"));
      startStage(STAGE_ZERO);
      break;

    case STAGE_ZERO:
      if (dispenser_process_result.steppers != AXIS_STATE_COMPLETE) break;
      Logger::log(F("Zero position reached, starting prime"));
      dispenserHead.x().reset();
      dispenserHead.y().reset();
      dispenserHead.z().reset();
      cycle = 0;
      startStage(STAGE_START_PRIME);
      break;

    case STAGE_WAIT_PRIME: {
      if (dispenser_process_result.dispenser != DISPENSER_STATE_IDLING) break;
      cycle++;

      // Check if priming should end
      if (cycle >= PRIME_DISPENSE_NUM) {
        Logger::log(F("Prime completed, moving to first position"));
        // Calculate the first position and move to it
        TrayHandler::Position first_position = trayHandler.reset();
        target_x =
            (profile.tray_origin_x +
             ((first_position.x - 1 + ((profile.staggered && first_position.y % 2 == 0) ? STAGGERED_OFFSET_FACTOR : 0)) * profile.pitch_x)) *
            -STEPS_PER_UNIT_X;
        target_y = (profile.tray_origin_y + ((first_position.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;
        startStage(STAGE_MOVE);
      } else {
        Logger::log(F("Prime not completed, starting next prime"));
        startStage(STAGE_START_PRIME);
      }
      break;
    }

    case STAGE_MOVE:
      if (dispenser_process_result.steppers != AXIS_STATE_COMPLETE) break;
      Logger::log(F("Move completed, lowering head"));
      startStage(STAGE_LOWER_HEAD);
      break;

    case STAGE_LOWER_HEAD: {
      if (dispenser_process_result.steppers != AXIS_STATE_COMPLETE) break;
      Logger::log(F("Head lowered, starting dispensing"));
      cycle = 0;
      startStage(STAGE_START_DISPENSE);
      MainScreenParams params = { profile, getRunStatus(), 0 };
      drawRunScreen(params);
      break;
    }

    case STAGE_WAIT_DISPENSE: {
      if (dispenser_process_result.dispenser != DISPENSER_STATE_IDLING) break;
      cycle++;

      // Check if dispensing should end
      if (cycle >= profile.cycles) {
        Logger::log(F("Dispensing completed, raising head"));
        dispenserHead.z().moveTo(0);
        startStage(STAGE_RAISE_HEAD);
      } else {
        Logger::log(F("Dispensing not completed, starting next dispensing"));
        startStage(STAGE_START_DISPENSE);
      }
      break;
    }

    case STAGE_RAISE_HEAD: {
      if (dispenser_process_result.steppers != AXIS_STATE_COMPLETE) break;
      TrayHandler::PositionResult result = trayHandler.goToNextValidPosition();
      snprintf(g_log_buffer, sizeof(g_log_buffer), "Has next: %d", result.has_next);
      Logger::log(g_log_buffer);
      snprintf(g_log_buffer, sizeof(g_log_buffer), "Next position: %d, %d", result.position.x, result.position.y);
      Logger::log(g_log_buffer);

      if (result.has_next) {
        Logger::log(F("Moving to next position"));
        snprintf(g_log_buffer, sizeof(g_log_buffer), "Next position: %d, %d", result.position.x, result.position.y);
        Logger::log(g_log_buffer);

        target_x = (profile.tray_origin_x +
                    ((result.position.x - 1 + ((profile.staggered && result.position.y % 2 == 0) ? STAGGERED_OFFSET_FACTOR : 0)) *
                     profile.pitch_x)) *
                   -STEPS_PER_UNIT_X;
        target_y = (profile.tray_origin_y + ((result.position.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;

        // Update Home_Screen
        MainScreenParams params = { profile, getRunStatus(), 0 };
        drawRunScreen(params);

        cycle = 0;
        startStage(STAGE_MOVE);
      } else {
        Logger::log(F("MODE: Run complete"));
        PlcSerial::sendCompleted();
        startNextController(CONTROLLER_HOMING);
      }
      break;
    }

    default:
      break;
  }
}

RunStatus RunController::getRunStatus() {
  return { (uint16_t)trayHandler.getCurrentRow(), (uint16_t)trayHandler.getCurrentColumn(),
           (uint16_t)trayHandler.getTubesLeft(), (uint16_t)trayHandler.getTubesDispensed() + 1 };
}

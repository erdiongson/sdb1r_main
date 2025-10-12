#include "RunController.h"
#include "../views/MainScreen.h"
#include "../views/common/Dialogs.h"
#include "../Utils.h"

RunController::RunController(ControllerParams params) : BaseController(params) {}

void RunController::onStart() {
  this->profile = profile_manager.getCurrentProfile();
  trayHandler.loadProfile(profile_manager.getCurrentProfile());
  TrayHandler::Position firstPosition = trayHandler.reset();

  if (firstPosition.x == -1 || firstPosition.y == -1) {
    Logger::log(F("MODE: No valid positions found, ending run mode"));
    startNextController(CONTROLLER_READY);
    return;
  }

  // Set the current position as the default 0 as a safe position
  // in the case when STOP is triggered before the Zeroing is completed
  dispenserHead.z().reset();

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
void RunController::startStage(Stage newStage) {
  switch (newStage) {
    case STAGE_IDLE:
      dispenserHead.x().stopRunning();
      dispenserHead.y().stopRunning();
      dispenserHead.z().stopRunning();
      break;

    case STAGE_SET_VIB_LEVEL:
      Logger::log(F("STAGE: Setting vibration level"));
      this->stage = STAGE_SET_VIB_LEVEL;
      dispenserHead.setVibrationLevel(profile.vibration_enabled);
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
      dispenserHead.z().moveToMin();
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
      Logger::log("STAGE: Moving to position X=" + String(target_x) + ", Y=" + String(target_y));
      this->stage = STAGE_MOVE;
      dispenserHead.x().moveTo(target_x);
      dispenserHead.y().moveTo(target_y);
      break;

    case STAGE_LOWER_HEAD:
      Logger::log("STAGE: Lowering head to Z=" + String(STEPS_PER_UNIT_Z * profile.z_dip));
      this->stage = STAGE_LOWER_HEAD;
      dispenserHead.z().moveTo(STEPS_PER_UNIT_Z * profile.z_dip);
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
      dispenserHead.z().moveTo(0);
      break;

    default:
      break;
  }
}

ControllerStepResult RunController::onStep() {
  if (paused) return ControllerStepResult(false);

  DispenserProcessResult dispenserProcessResult = dispenserHead.process();
  if (dispenserProcessResult.steppers == AXIS_STATE_RUNNING) {
    return ControllerStepResult(true);
  }

  MainScreenParams params = { profile,
                              { (uint16_t)trayHandler.getCurrentRow(), (uint16_t)trayHandler.getCurrentColumn(),
                                (uint16_t)trayHandler.getTubesLeft(), (uint16_t)trayHandler.getTubesDispensed() + 1 },
                              0 };

  if (dispenserProcessResult.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    Logger::log(F("MODE: Stepper error - limit switch triggered"));
    params.dialog_code = DIALOG_ERROR_LIMIT_SWITCH;
    drawRunScreen(params);
    pause();
    return ControllerStepResult(false);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    Logger::log(F("MODE: Dispenser error - IR sensor failure"));
    params.dialog_code = DIALOG_ERROR_IR_SENSOR;
    drawRunScreen(params);
    pause();
    return ControllerStepResult(false);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    Logger::log(F("MODE: Dispenser error - Acknowledgment error"));
    params.dialog_code = DIALOG_ERROR_ACK_ERROR;
    drawRunScreen(params);
    pause();
    return ControllerStepResult(false);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    Logger::log(F("MODE: Dispenser error - marker not detected"));
    params.dialog_code = DIALOG_ERROR_MARKER_NOT_DETECTED;
    drawRunScreen(params);
    pause();

    return ControllerStepResult(false);
  }

  // Perform logic after all axis are idle (all movement is completed)
  processStageLogic(dispenserProcessResult);

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
  if (interaction.plc_message_type == MSG_STOP || interaction.key_pressed == STOP) {
    stop();
    return;
  }

  if (interaction.plc_message_type == MSG_PAUSE || interaction.key_pressed == PAUSE) {
    if (paused) {
      start();
    } else {
      pause();
    }
    return;
  }
}

// Processes the stage logic based on current stage and dispenser state
void RunController::processStageLogic(DispenserProcessResult& dispenserProcessResult) {
  switch (stage) {
    case STAGE_SET_VIB_LEVEL:
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      startStage(STAGE_SET_VIB_DURATION);
      break;

    case STAGE_SET_VIB_DURATION:
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      startStage(STAGE_ZERO);
      break;

    case STAGE_ZERO:
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;
      cycle = 0;
      startStage(STAGE_START_PRIME);
      break;

    case STAGE_WAIT_PRIME: {
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      cycle++;

      // Check if priming should end
      if (cycle >= PRIME_DISPENSE_NUM) {
        // Calculate the first position and move to it
        TrayHandler::Position firstPosition = trayHandler.reset();
        target_x =
            (profile.tray_origin_x +
             ((firstPosition.x - 1 + ((profile.staggered && firstPosition.y % 2 == 0) ? 0.5 : 0)) * profile.pitch_x)) *
            -STEPS_PER_UNIT_X;
        target_y = (profile.tray_origin_y + ((firstPosition.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;
        startStage(STAGE_MOVE);
      } else {
        startStage(STAGE_START_PRIME);
      }
      break;
    }

    case STAGE_MOVE:
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;
      startStage(STAGE_LOWER_HEAD);
      break;

    case STAGE_LOWER_HEAD: {
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;
      cycle = 0;
      startStage(STAGE_START_DISPENSE);
      break;
    }

    case STAGE_WAIT_DISPENSE: {
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      cycle++;

      // Check if dispensing should end
      if (cycle >= profile.cycles) {
        dispenserHead.z().moveTo(0);
        startStage(STAGE_RAISE_HEAD);
      } else {
        startStage(STAGE_START_DISPENSE);
      }
      break;
    }

    case STAGE_RAISE_HEAD: {
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;
      TrayHandler::PositionResult result = trayHandler.goToNextValidPosition();
      Logger::log("Has next: " + String(result.has_next));
      Logger::log("Next position: " + String(result.position.x) + ", " + String(result.position.y));

      if (result.has_next) {
        Logger::log("Next position: " + String(result.position.x) + ", " + String(result.position.y));

        target_x = (profile.tray_origin_x +
                    ((result.position.x - 1 + ((profile.staggered && result.position.y % 2 == 0) ? 0.5 : 0)) *
                     profile.pitch_x)) *
                   -STEPS_PER_UNIT_X;
        target_y = (profile.tray_origin_y + ((result.position.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;

        // Update Home_Screen
        MainScreenParams params = { profile,
                                    { (uint16_t)trayHandler.getCurrentRow(), (uint16_t)trayHandler.getCurrentColumn(),
                                      (uint16_t)trayHandler.getTubesLeft(),
                                      (uint16_t)trayHandler.getTubesDispensed() + 1 },
                                    0 };
        drawRunScreen(params);

        cycle = 0;
        startStage(STAGE_MOVE);
      } else {
        Logger::log(F("MODE: Run complete"));
        startNextController(CONTROLLER_READY);
      }
      break;
    }

    default:
      break;
  }
}

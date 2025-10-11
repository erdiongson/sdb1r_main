#include "RunController.h"
#include "../views/MainScreen.h"
#include "../views/common/Dialogs.h"

RunController::RunController(ControllerParams params)
  : BaseController(params) {}

void RunController::onStart(Profile& profile) {
  this->profile = profile;
  trayHandler.loadProfile(profile);
  TrayHandler::Position firstPosition = trayHandler.reset();

  if (firstPosition.x == -1 || firstPosition.y == -1) {
    Serial.println(F("MODE: No valid positions found, ending run mode"));
    startNextController(CONTROLLER_READY);
    return;
  }

  // Set the current position as the default 0 as a safe position
  // in the case when STOP is triggered before the Zeroing is completed
  dispenserHead.z().reset();

  // Actually start the sequence
  start();
}

void RunController::onInteraction(const Interaction& interaction) {
  int button = interaction.key_pressed;

  if (button == PAUSE || interaction.plc_message_type == MSG_PAUSE) {
    drawMainScreen(phost, PAUSEMENU);
    pause();
  } else if (button == STOP || interaction.plc_message_type == MSG_STOP) {
    stop();
  } else if (button == START) {
    start();
  }
}

// Pauses the run by stopping all axis movements immediately.
void RunController::pause() {
  Serial.println(F("MODE: Paused"));
  paused = true;
  dispenserHead.x().stop();
  dispenserHead.y().stop();
  dispenserHead.z().stop();
}

// Stops the run and returns to home position.
void RunController::stop() {
  Serial.println(F("MODE: Stopped"));
  drawMainScreen(phost, STOPPINGMENU);
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

// Resumes the run from a paused state.
void RunController::start() {
  Serial.println(F("MODE: Resumed"));
  paused = false;
  drawMainScreen(phost, RUNMENU);
  startStage(stage);
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
      Serial.println(F("STAGE: Setting vibration level"));
      this->stage = STAGE_SET_VIB_LEVEL;
      dispenserHead.setVibrationLevel(profile.vibrationEnabled);
      break;

    case STAGE_SET_VIB_DURATION:
      Serial.print(F("STAGE: Setting vibration duration to "));
      Serial.println(profile.vibrationDuration);
      this->stage = STAGE_SET_VIB_DURATION;
      dispenserHead.setVibrationTime(profile.vibrationDuration);
      break;

    case STAGE_ZERO:
      Serial.println(F("STAGE: Moving to zero position"));
      this->stage = STAGE_ZERO;
      dispenserHead.x().moveToMax();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMin();
      break;

    case STAGE_START_PRIME:
      Serial.println(F("STAGE: Starting prime"));
      this->stage = STAGE_START_PRIME;
      dispenserHead.sendDispense();

      // Immediately move to wait stage
      startStage(STAGE_WAIT_PRIME);
      break;

    case STAGE_WAIT_PRIME:
      Serial.println(F("STAGE: Waiting for prime to complete"));
      this->stage = STAGE_WAIT_PRIME;
      break;

    case STAGE_MOVE:
      Serial.print(F("STAGE: Moving to position X="));
      Serial.print(target_x);
      Serial.print(F(", Y="));
      Serial.println(target_y);
      this->stage = STAGE_MOVE;
      dispenserHead.x().moveTo(target_x);
      dispenserHead.y().moveTo(target_y);
      break;

    case STAGE_LOWER_HEAD:
      Serial.print(F("STAGE: Lowering head to Z="));
      Serial.println(STEPS_PER_UNIT_Z * profile.ZDip);
      this->stage = STAGE_LOWER_HEAD;
      dispenserHead.z().moveTo(STEPS_PER_UNIT_Z * profile.ZDip);
      break;

    case STAGE_START_DISPENSE:
      Serial.println(F("STAGE: Starting dispensing"));
      this->stage = STAGE_START_DISPENSE;
      dispenserHead.sendDispense();

      // Immediately move to wait stage
      startStage(STAGE_WAIT_DISPENSE);
      break;

    case STAGE_WAIT_DISPENSE:
      Serial.println(F("STAGE: Waiting for dispensing to complete"));
      this->stage = STAGE_WAIT_DISPENSE;
      break;

    case STAGE_RAISE_HEAD:
      Serial.println(F("STAGE: Raising head"));
      this->stage = STAGE_RAISE_HEAD;
      dispenserHead.z().moveTo(0);
      break;

    default:
      break;
  }
}

// Check if the current stage is complete, and move to the next stage if required
void RunController::processStageLogic(DispenserProcessResult& dispenserProcessResult) {
  switch (stage) {
    case STAGE_IDLE:
      startStage(STAGE_SET_VIB_LEVEL);
      break;

    case STAGE_SET_VIB_LEVEL:
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      startStage(STAGE_SET_VIB_DURATION);
      break;

    case STAGE_SET_VIB_DURATION:
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      startStage(STAGE_ZERO);
      break;

    case STAGE_ZERO: {
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;

      // Set the current Axes positions as 0
      dispenserHead.x().reset();
      dispenserHead.y().reset();
      dispenserHead.z().reset();

      cycle = 0;
      startStage(STAGE_START_PRIME);
      break;
    }

    case STAGE_WAIT_PRIME: {
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      cycle++;

      // Check if priming should end
      if (cycle >= PRIME_DISPENSE_NUM) {
        // Calculate the first position and move to it
        TrayHandler::Position firstPosition = trayHandler.reset();
        target_x = (profile.trayOriginX + ((firstPosition.x - 1 + ((profile.staggered && firstPosition.y % 2 == 0) ? 0.5 : 0)) * profile.pitch_x)) * -STEPS_PER_UNIT_X;
        target_y = (profile.trayOriginY + ((firstPosition.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;
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
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING)  break;
      cycle++;

      // Check if dispensing should end
      if (cycle >= profile.Cycles) {
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
      Serial.println("Has next: " + String(result.hasNext));
      Serial.println("Next position: " + String(result.position.x) + ", " + String(result.position.y));

      if (result.hasNext) {
        Serial.println("Next position: " + String(result.position.x) + ", " + String(result.position.y));

        target_x = (profile.trayOriginX + ((result.position.x - 1 + ((profile.staggered && result.position.y % 2 == 0) ? 0.5: 0)) * profile.pitch_x)) * -STEPS_PER_UNIT_X;
        target_y = (profile.trayOriginY + ((result.position.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;

        // Update Home_Screen
        MainScreenParams params = {
          (uint16_t)trayHandler.getCurrentRow(),
          (uint16_t)trayHandler.getCurrentColumn(),
          (uint16_t)trayHandler.getTubesLeft(),
          (uint16_t)trayHandler.getTubesDispensed() + 1,
          0
        };
        drawMainScreen(phost, RUNMENU, &params);
        startStage(STAGE_MOVE);
      } else {
        startNextController(CONTROLLER_HOMING);
      }
      break;
    }

    default:
      break;
  }
}

 ControllerStepResult RunController::onStep() {
  if (paused) return ControllerStepResult(-1, -1);

  DispenserProcessResult dispenserProcessResult = dispenserHead.process();
  if (dispenserProcessResult.steppers == AXIS_STATE_RUNNING) {
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  MainScreenParams params = {
    (uint16_t)trayHandler.getCurrentRow(),
    (uint16_t)trayHandler.getCurrentColumn(),
    (uint16_t)trayHandler.getTubesLeft(),
    (uint16_t)trayHandler.getTubesDispensed() + 1,
    0
  };

  if (dispenserProcessResult.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    Serial.println(F("MODE: Stepper error - limit switch triggered"));
    params.error_code = DIALOG_ERROR_LIMIT_SWITCH;
    drawMainScreen(phost, RUNMENU, &params);
    pause();
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    Serial.println(F("MODE: Dispenser error - IR sensor failure"));
    params.error_code = DIALOG_ERROR_IR_SENSOR;
    drawMainScreen(phost, RUNMENU, &params);
    pause();
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    Serial.println(F("MODE: Dispenser error - Acknowledgment error"));
    params.error_code = DIALOG_ERROR_ACK_ERROR;
    drawMainScreen(phost, RUNMENU, &params);
    pause();
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    Serial.println(F("MODE: Dispenser error - marker not detected"));
    params.error_code = DIALOG_ERROR_MARKER_NOT_DETECTED;
    drawMainScreen(phost, RUNMENU, &params);
    pause();
    
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  // Perform logic after all axis are idle (all movement is completed)
  processStageLogic(dispenserProcessResult);

  return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
}

int RunController::getModeType() const {
  return CONTROLLER_RUN;
}

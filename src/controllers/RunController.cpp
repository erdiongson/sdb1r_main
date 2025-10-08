#include "RunController.h"
#include "../views/HomeScreen.h"
#include "../views/Dialogs.h"

RunController::RunController(ControllerParams params)
  : BaseController(params) {}

void RunController::on_start(Profile& profile) {
  this->profile = profile;
  trayHandler.load_profile(profile);
  TrayHandler::Position firstPosition = trayHandler.reset();

  if (firstPosition.x == -1 || firstPosition.y == -1) {
    Serial.println(F("MODE: No valid positions found, ending run mode"));
    start_next_controller(CONTROLLER_HOME);
    return;
  }

  draw_home_screen(phost, RUNMENU);
  // on_step will trigger process_stage_logic to transition into SET_VIB_LEVEL_STAGE
}

void RunController::on_interaction(const Interaction& interaction) {
  int button = interaction.key_pressed;

  if (button == PAUSE || interaction.plc_message_type == MSG_PAUSE) {
    Serial.println(F("MODE: Paused"));
    paused = true;
    dispenserHead.x().stop();
    dispenserHead.y().stop();
    dispenserHead.z().stop();

    draw_home_screen(phost, PAUSEMENU);
  } else if (button == STOP || interaction.plc_message_type == MSG_STOP) {
    Serial.println(F("MODE: Stopped"));
    draw_home_screen(phost, STOPPINGMENU);
    paused = false;

    dispenserHead.x().stopRunning();
    dispenserHead.y().stopRunning();
    dispenserHead.z().stopRunning();

    // Wait until dispenser is finished with any ongoing action
    while (dispenserHead.get_state() != DISPENSER_STATE_IDLING) {
      dispenserHead.process();
    }

    start_stage(HOME_STAGE);
  } else if (button == START) {
    Serial.println(F("MODE: Resumed"));
    paused = false;
    draw_home_screen(phost, RUNMENU);
    start_stage(stage);
  }
}

void RunController::start_stage(Stage newStage) {
  switch (newStage) {
    case SET_VIB_LEVEL_STAGE:
      Serial.println(F("STAGE: Setting vibration level"));
      this->stage = SET_VIB_LEVEL_STAGE;
      dispenserHead.set_vibration_level(profile.vibrationEnabled);
      break;

    case SET_VIB_DURATION_STAGE:
      Serial.print(F("STAGE: Setting vibration duration to "));
      Serial.println(profile.vibrationDuration);
      this->stage = SET_VIB_DURATION_STAGE;
      dispenserHead.set_vibration_time(profile.vibrationDuration);
      break;

    case ZERO_STAGE:
      Serial.println(F("STAGE: Moving to zero position"));
      this->stage = ZERO_STAGE;
      dispenserHead.x().moveToMax();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMin();
      break;

    case PRIME_STAGE:
      Serial.println(F("STAGE: Priming dispenser"));
      this->stage = PRIME_STAGE;
      if (cycle < PRIME_DISPENSE_NUM) {
        dispenserHead.send_dispense();
      }
      break;

    case MOVE_STAGE:
      Serial.print(F("STAGE: Moving to position X="));
      Serial.print(target_x);
      Serial.print(F(", Y="));
      Serial.println(target_y);
      this->stage = MOVE_STAGE;
      dispenserHead.x().moveTo(target_x);
      dispenserHead.y().moveTo(target_y);
      break;

    case LOWER_HEAD_STAGE:
      Serial.print(F("STAGE: Lowering head to Z="));
      Serial.println(STEPS_PER_UNIT_Z * profile.ZDip);
      this->stage = LOWER_HEAD_STAGE;
      dispenserHead.z().moveTo(STEPS_PER_UNIT_Z * profile.ZDip);
      break;

    case DISPENSE_STAGE:
      Serial.println(F("STAGE: Dispensing"));
      this->stage = DISPENSE_STAGE;
      dispenserHead.send_dispense();
      break;

    case RAISE_HEAD_STAGE:
      Serial.println(F("STAGE: Raising head"));
      this->stage = RAISE_HEAD_STAGE;
      dispenserHead.z().moveTo(0);
      break;

    case HOME_STAGE:
      Serial.println(F("STAGE: Returning to home position"));
      this->stage = HOME_STAGE;
      draw_home_screen(phost, HOMINGMENU);
      dispenserHead.x().moveToMin();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMin();
      break;

    default:
      break;
  }
}

void RunController::process_stage_logic(DispenserProcessResult& dispenserProcessResult) {
  switch (stage) {
    case IDLE_STAGE:
      start_stage(SET_VIB_LEVEL_STAGE);
      break;

    case SET_VIB_LEVEL_STAGE:
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      start_stage(SET_VIB_DURATION_STAGE);
      break;

    case SET_VIB_DURATION_STAGE:
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      start_stage(ZERO_STAGE);
      break;

    case ZERO_STAGE: {
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;

      // Set the current Axes positions as 0
      dispenserHead.x().reset();
      dispenserHead.y().reset();
      dispenserHead.z().reset();

      cycle = 0;
      start_stage(PRIME_STAGE);
      break;
    }

    case PRIME_STAGE: {
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING) break;
      cycle++;

      // Check if priming should end
      if (cycle >= PRIME_DISPENSE_NUM) {
        // Calculate the first position and move to it
        TrayHandler::Position firstPosition = trayHandler.reset();
        target_x = (profile.trayOriginX + ((firstPosition.x - 1 + ((profile.staggered && firstPosition.y % 2 == 0) ? 0.5 : 0)) * profile.pitch_x)) * -STEPS_PER_UNIT_X;
        target_y = (profile.trayOriginY + ((firstPosition.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;
        start_stage(MOVE_STAGE);
      } else {
        dispenserHead.send_dispense();
        start_stage(PRIME_STAGE);
      }
      break;
    }

    case MOVE_STAGE:
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;
      start_stage(LOWER_HEAD_STAGE);
      break;

    case LOWER_HEAD_STAGE: {
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;
      cycle = 0;
      start_stage(DISPENSE_STAGE);
      break;
    }

    case DISPENSE_STAGE: {
      if (dispenserProcessResult.dispenser != DISPENSER_STATE_IDLING)  break;
      cycle++;

      // Check if dispensing should end
      if (cycle >= profile.Cycles) {
        dispenserHead.z().moveTo(0);
        start_stage(RAISE_HEAD_STAGE);
      } else {
        dispenserHead.send_dispense();
        start_stage(DISPENSE_STAGE);
      }
      break;
    }

    case RAISE_HEAD_STAGE: {
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;
      TrayHandler::PositionResult result = trayHandler.goToNextValidPosition();
      Serial.println("Has next: " + String(result.hasNext));
      Serial.println("Next position: " + String(result.position.x) + ", " + String(result.position.y));

      if (result.hasNext) {
        Serial.println("Next position: " + String(result.position.x) + ", " + String(result.position.y));

        target_x = (profile.trayOriginX + ((result.position.x - 1 + ((profile.staggered && result.position.y % 2 == 0) ? 0.5: 0)) * profile.pitch_x)) * -STEPS_PER_UNIT_X;
        target_y = (profile.trayOriginY + ((result.position.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;

        // Update Home_Screen
        HomeParams params = {
          (uint16_t)trayHandler.getCurrentRow(),
          (uint16_t)trayHandler.getCurrentColumn(),
          (uint16_t)trayHandler.getTubesLeft(),
          0
        };
        draw_home_screen(phost, RUNMENU, &params);
        start_stage(MOVE_STAGE);
      } else {
        start_stage(HOME_STAGE);
      }
      break;
    }

    case HOME_STAGE:
      if (dispenserProcessResult.steppers != AXIS_STATE_COMPLETE) break;
      start_next_controller(CONTROLLER_HOME);
      break;

    default:
      break;
  }
}

ControllerStepResult RunController::on_step() {
  if (paused) return ControllerStepResult(-1, -1);

  DispenserProcessResult dispenserProcessResult = dispenserHead.process();
  if (dispenserProcessResult.steppers == AXIS_STATE_RUNNING) {
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  HomeParams params = {
    (uint16_t)trayHandler.getCurrentRow(),
    (uint16_t)trayHandler.getCurrentColumn(),
    (uint16_t)trayHandler.getTubesLeft(),
  };

  if (dispenserProcessResult.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    Serial.println(F("MODE: Stepper error - limit switch triggered"));
    draw_home_screen(phost, RUNMENU, &params);
    start_stage(HOME_STAGE);
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    Serial.println(F("MODE: Dispenser error - IR sensor failure"));
    params.error_code = DIALOG_ERROR_IR_SENSOR;
    draw_home_screen(phost, RUNMENU, &params);
    start_stage(HOME_STAGE);
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    Serial.println(F("MODE: Dispenser error - Acknowledgment error"));
    params.error_code = DIALOG_ERROR_ACK_ERROR;
    draw_home_screen(phost, RUNMENU, &params);
    start_stage(HOME_STAGE);
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    Serial.println(F("MODE: Dispenser error - marker not detected"));
    params.error_code = DIALOG_ERROR_MARKER_NOT_DETECTED;
    draw_home_screen(phost, RUNMENU, &params);
    start_stage(HOME_STAGE);
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  // Perform logic after all axis are idle (all movement is completed)
  process_stage_logic(dispenserProcessResult);

  return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
}

int RunController::get_mode_type() const {
  return CONTROLLER_RUN;
}

#include "RunController.h"
#include "../views/HomeScreen.h"
#include "../views/ErrorMessages.h"

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
    start_next_controller(CONTROLLER_HOME);
  } else if (button == START) {
    Serial.println(F("MODE: Resumed"));
    paused = false;
    draw_home_screen(phost, RUNMENU);
  }
}

void RunController::process_stage_logic(DispenserProcessResult& dispenserProcessResult) {
  switch (stage) {
    case IDLE_STAGE:
      dispenserHead.set_vibration_level(profile.vibrationEnabled);

      Serial.println(F("MODE: IDLE STAGE -> SET_VIBRATION_LEVEL STAGE"));
      this->stage = SET_VIB_LEVEL_STAGE;
      break;

    case SET_VIB_LEVEL_STAGE:
      if (dispenserProcessResult.dispenser == DISPENSER_STATE_IDLING) {
        dispenserHead.set_vibration_time(profile.vibrationDuration);

        Serial.println(F("MODE: SET_VIBRATION_LEVEL STAGE -> SET_VIBRATION_TIME STAGE"));
        this->stage = SET_VIB_DURATION_STAGE;
      }
      break;

    case SET_VIB_DURATION_STAGE:
      Serial.println(F("MODE: SET_VIBRATION_TIME STAGE -> ZERO STAGE"));

      dispenserHead.x().moveToMax();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMin();

      this->stage = ZERO_STAGE;
      break;

    case ZERO_STAGE:
      Serial.println(F("MODE: ZERO STAGE -> MOVE STAGE"));
      // Set the current Axes positions as 0
      dispenserHead.x().reset();
      dispenserHead.y().reset();
      dispenserHead.z().reset();

      cycle = 0;
      dispenserHead.send_dispense();
      this->stage = PRIME_STAGE;
      break;

    case PRIME_STAGE:
      if (dispenserProcessResult.dispenser == DISPENSER_STATE_IDLING) {
        cycle++;

        // Check if priming should end
      if (cycle >= profile.Cycles) {
          // Calculate the first position and move to it
          TrayHandler::Position firstPosition = trayHandler.reset();
          target_x = (profile.trayOriginX + ((firstPosition.x - 1 + (profile.staggered ? 0.5 : 0)) * profile.pitch_x)) * -STEPS_PER_UNIT_X;
          target_y = (profile.trayOriginY + ((firstPosition.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;

          dispenserHead.x().moveTo(target_x);
          dispenserHead.y().moveTo(target_y);

          Serial.println(F("MODE: PRIME_STAGE -> MOVE STAGE"));
          this->stage = MOVE_STAGE;
        } else {
          dispenserHead.send_dispense();

          Serial.println(F("MODE: PRIME_STAGE -> PRIME STAGE"));
          this->stage = PRIME_STAGE;
        }
      }
      break;

    case MOVE_STAGE:
      Serial.println(F("MODE: MOVE STAGE -> LOWER_HEAD STAGE"));
      dispenserHead.z().moveTo(STEPS_PER_UNIT_Z * profile.ZDip);
      this->stage = LOWER_HEAD_STAGE;
      break;

    case LOWER_HEAD_STAGE:
      Serial.println(F("MODE: LOWER_HEAD STAGE -> START_DISPENSE STAGE"));
      cycle = 0;
      dispenserHead.send_dispense();
      this->stage = START_DISPENSE_STAGE;
      break;

    case DISPENSE_STAGE:
      if (dispenserProcessResult.dispenser == DISPENSER_STATE_IDLING) {
        cycle++;

        // Check if dispensing should end
        if (cycle >= profile.Cycles) {
          dispenserHead.z().moveTo(0);
          Serial.println(F("MODE: DISPENSE STAGE -> RAISE_HEAD STAGE"));
          this->stage = RAISE_HEAD_STAGE;
        } else {
          Serial.println(F("MODE: DISPENSE STAGE -> DISPENSE STAGE"));
          dispenserHead.send_dispense();
          this->stage = DISPENSE_STAGE;
        }
      }
      break;

    case RAISE_HEAD_STAGE:
      TrayHandler::PositionResult result = trayHandler.goToNextValidPosition();

      if (result.hasNext) {
        Serial.println("Next position: " + String(result.position.x) + ", " + String(result.position.y));

        target_x = (profile.trayOriginX + ((result.position.x - 1 + (profile.staggered ? 0.5: 0)) * profile.pitch_x)) * -STEPS_PER_UNIT_X;
        target_y = (profile.trayOriginY + ((result.position.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;

        // Update Home_Screen
        HomeParams params = {
          (uint16_t)trayHandler.getCurrentRow(),
          (uint16_t)trayHandler.getCurrentColumn(),
          (uint16_t)trayHandler.getTubesLeft(),
          0
        };
        draw_home_screen(phost, RUNMENU, &params);
        Serial.println(F("MODE: RAISE_HEAD STAGE -> MOVE STAGE"));
        this->stage = MOVE_STAGE;

        dispenserHead.x().moveTo(target_x);
        dispenserHead.y().moveTo(target_y);
      } else {
        Serial.println(F("MODE: RAISE_HEAD STAGE -> HOME STAGE"));
        this->stage = HOME_STAGE;
      }
      break;

    case HOME_STAGE:
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
    params.error_code = ERROR_LIMIT_SWITCH;
    draw_home_screen(phost, RUNMENU, &params);
    // start_stage(HOME_STAGE);
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    Serial.println(F("MODE: Dispenser error - IR sensor failure"));
    params.error_code = ERROR_IR_SENSOR;
    draw_home_screen(phost, RUNMENU, &params);
    // start_stage(HOME_STAGE);
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_ACK_ERROR) {
    Serial.println(F("MODE: Dispenser error - Acknowledgment error"));
    params.error_code = ERROR_ACK_ERROR;
    draw_home_screen(phost, RUNMENU, &params);
    // start_stage(HOME_STAGE);
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    Serial.println(F("MODE: Dispenser error - marker not detected"));
    params.error_code = ERROR_MARKER_NOT_DETECTED;
    draw_home_screen(phost, RUNMENU, &params);
    // start_stage(HOME_STAGE);
    return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  // Perform logic after all axis are idle (all movement is completed)
  process_stage_logic(dispenserProcessResult);

  return ControllerStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
}

int RunController::get_mode_type() const {
  return CONTROLLER_RUN;
}

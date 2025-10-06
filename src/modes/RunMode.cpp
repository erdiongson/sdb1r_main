#include "RunMode.h"
#include "ModesCommon.h"
#include "../views/Home_Screen.h"
#include "../views/Error_Messages.h"

RunMode::RunMode(DispenserHead& head, Gpu_Hal_Context_t* host, ModeController* controller, ModeCompletionCallback callback)
  : BaseMode(head, host, controller, callback) {}

void RunMode::on_start(Profile& profile) {
  Serial.println(F("MODE: Setting profile on trayhandler"));
  trayHandler.load_profile(profile);
  TrayHandler::Position firstPosition = trayHandler.reset();

  if (firstPosition.x == -1 || firstPosition.y == -1) {
    Serial.println(F("MODE: No valid positions found, ending run mode"));
    complete_with_next_mode(MODE_TYPE_HOME);
    return;
  }

  Serial.println(F("MODE: Setting profile"));
  this->profile = profile;

  paused = false;

  Home_Screen(phost, RUNMENU);
  start_stage(ZERO_STAGE);
}

void RunMode::on_interaction(const Interaction& interaction) {
  int button = interaction.key_pressed;

  if (button == PAUSE || interaction.plc_message_type == MSG_PAUSE) {
    Serial.println(F("MODE: Paused"));
    paused = true;
    dispenserHead.x().stop();
    dispenserHead.y().stop();
    dispenserHead.z().stop();

    Home_Screen(phost, PAUSEMENU);
  } else if (button == STOP || interaction.plc_message_type == MSG_STOP) {
    Serial.println(F("MODE: Stopped"));
    complete_with_next_mode(MODE_TYPE_HOME);
  } else if (button == START) {
    Serial.println(F("MODE: Resumed"));
    paused = false;
    Home_Screen(phost, RUNMENU);
    start_stage(stage);
  }
}

ModeStepResult RunMode::on_step() {
  if (paused) return ModeStepResult(MODE_CONTINUE, MODE_CONTINUE);

  DispenserProcessResult dispenserProcessResult = dispenserHead.process();
  if (dispenserProcessResult.steppers == AXIS_STATE_RUNNING) {
    return ModeStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  HomeParams params = {
    (uint16_t)trayHandler.getCurrentRow(),
    (uint16_t)trayHandler.getCurrentColumn(),
    (uint16_t)trayHandler.getTubesLeft(),
  };

  if (dispenserProcessResult.steppers == AXIS_STATE_ERROR_LIMIT_SWITCH) {
    Serial.println(F("MODE: Stepper error - limit switch triggered"));
    params.error_code = ERROR_LIMIT_SWITCH;
    Home_Screen(phost, RUNMENU, &params);
    start_stage(HOME_STAGE);
    return ModeStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE) {
    Serial.println(F("MODE: Dispenser error - IR sensor failure"));
    params.error_code = ERROR_IR_SENSOR;
    Home_Screen(phost, RUNMENU, &params);
    start_stage(HOME_STAGE);
    return ModeStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    Serial.println(F("MODE: Dispenser error - marker not detected"));
    params.error_code = ERROR_MARKER_NOT_DETECTED;
    Home_Screen(phost, RUNMENU, &params);
    start_stage(HOME_STAGE);
    return ModeStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  // Perform logic after all axis are idle (all movement is completed)
  process_stage_logic();

  return ModeStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
}

void RunMode::process_stage_logic() {
  switch (stage) {
    case IDLE_STAGE:
      Serial.println(F("MODE: IDLE STAGE -> ZERO STAGE"));
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
        if (cycle >= profile.cycles) {
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
        if (cycle >= profile.cycles) {
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
        Home_Screen(phost, RUNMENU, &params);
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
      complete_with_next_mode(MODE_TYPE_HOME);
      break;

    default:
      break;
  }
}

int RunMode::get_mode_type() const {
  return MODE_TYPE_RUN;
}

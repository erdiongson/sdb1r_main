#include "RunMode.h"
#include "ModesCommon.h"
#include "../views/Home_Screen.h"

RunMode::RunMode(DispenserHead& head, Gpu_Hal_Context_t* host, ModeController* controller, ModeCompletionCallback callback)
  : BaseMode(head, host, controller, callback) {}

void RunMode::on_start(Profile& profile) {
  Serial.println(F("MODE: Setting profile on trayhandler"));
  trayHandler.load_profile(profile);
  trayHandler.reset();

  Serial.println(F("MODE: Setting profile"));
  this->profile = profile;

  paused = false;

  Home_Screen(phost, RUNMENU);
  start_stage(ZERO_STAGE);
}

void RunMode::on_interaction(const Interaction& interaction) {
  int button = interaction.key_pressed;

  if (button == PAUSE) {
    Serial.println(F("MODE: Paused"));
    paused = true;
    dispenserHead.x().stop();
    dispenserHead.y().stop();
    dispenserHead.z().stop();

    Home_Screen(phost, PAUSEMENU);
  } else if (button == STOP) {
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
  if (dispenserProcessResult.steppers == AXIS_STATE_RUNNING) return ModeStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);

  if (dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE || dispenserProcessResult.dispenser == DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED) {
    Serial.println(F("MODE: Dispenser error - IR sensor failure or marker not detected"));
    HomeParams params = {
      (uint16_t)trayHandler.getCurrentRow(),
      (uint16_t)trayHandler.getCurrentColumn(),
      (uint16_t)trayHandler.getTubesLeft(),
      (uint8_t)dispenserProcessResult.dispenser
    };
    Home_Screen(phost, RUNMENU, &params);
    start_stage(HOME_STAGE);
    return ModeStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
  }

  switch (stage) {
    case ZERO_STAGE:
      dispenserHead.x().reset();
      dispenserHead.y().reset();
      dispenserHead.z().reset();

      start_stage(OFFSET_STAGE);
      break;
    case OFFSET_STAGE:
      start_stage(LOWER_HEAD_STAGE);
      break;
    case LOWER_HEAD_STAGE:
      start_stage(START_DISPENSE_STAGE);
      break;
    case START_DISPENSE_STAGE:
      if (dispenserProcessResult.dispenser == DISPENSER_STATE_ACKNOWLEDGED) {
        start_stage(WAIT_DISPENSE_STAGE);
      }
      break;
    case WAIT_DISPENSE_STAGE:
      if (dispenserProcessResult.dispenser == DISPENSER_STATE_IDLING) {
        start_stage(RAISE_HEAD_STAGE);
      }
      break;
    case RAISE_HEAD_STAGE: {
        TrayHandler::PositionResult result = trayHandler.goToNextValidPosition();
        if (result.hasNext) {
          Serial.println("Next position: " + String(result.position.x) + ", " + String(result.position.y));

          target_x = (profile.trayOriginX + ((result.position.x - 1) * profile.pitch_x)) * -STEPS_PER_UNIT_X;
          target_y = (profile.trayOriginY + ((result.position.y - 1) * profile.pitch_y)) * STEPS_PER_UNIT_Y;

          // Update Home_Screen
          HomeParams params = {
            (uint16_t)trayHandler.getCurrentRow(),
            (uint16_t)trayHandler.getCurrentColumn(),
            (uint16_t)trayHandler.getTubesLeft(),
            0
          };
          Home_Screen(phost, RUNMENU, &params);
          start_stage(MOVE_STAGE);
        } else {
          start_stage(HOME_STAGE);
        }
      }
      break;
    case MOVE_STAGE:
      start_stage(LOWER_HEAD_STAGE);
      break;
    case HOME_STAGE:
      stage = IDLE_STAGE;
      return ModeStepResult(MODE_COMPLETE, MODE_COMPLETE);
      break;
    default:
      break;
  }
  return ModeStepResult(dispenserProcessResult.steppers, dispenserProcessResult.dispenser);
}

void RunMode::start_stage(int newStage) {
  switch (newStage) {
    case ZERO_STAGE:
      Serial.println(F("MODE: Setting stage: ZERO"));
      this->stage = ZERO_STAGE;
      dispenserHead.x().moveToMax();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMin();
      break;
    case OFFSET_STAGE:
      Serial.println(F("MODE: Setting stage: OFFSET"));
      this->stage = OFFSET_STAGE;

      dispenserHead.x().moveTo(STEPS_PER_UNIT_X * -profile.trayOriginX);
      dispenserHead.y().moveTo(STEPS_PER_UNIT_Y * profile.trayOriginY);
      break;
    case LOWER_HEAD_STAGE:
      Serial.println(F("MODE: Setting stage: LOWER_HEAD"));
      this->stage = LOWER_HEAD_STAGE;
      dispenserHead.z().moveTo(STEPS_PER_UNIT_Z * profile.ZDip);
      break;
    case START_DISPENSE_STAGE:
      Serial.println(F("MODE: Setting stage: START_DISPENSE"));
      this->stage = START_DISPENSE_STAGE;
      dispenserHead.send_dispense();
      break;
    case WAIT_DISPENSE_STAGE:
      Serial.println(F("MODE: Setting stage: WAIT_DISPENSE"));
      this->stage = WAIT_DISPENSE_STAGE;
      break;
    case RAISE_HEAD_STAGE:
      Serial.println(F("MODE: Setting stage: RAISE_HEAD"));
      this->stage = RAISE_HEAD_STAGE;
      dispenserHead.z().moveTo(0);
      break;
    case MOVE_STAGE:
      Serial.println(F("MODE: Setting stage: MOVE"));
      this->stage = MOVE_STAGE;

      Serial.println(F("Setting target x and target y!"));
      dispenserHead.x().moveTo(target_x);
      dispenserHead.y().moveTo(target_y);
      break;
    case HOME_STAGE:
      Serial.println(F("MODE: Setting stage: HOME"));
      this->stage = HOME_STAGE;
      dispenserHead.x().moveToMin();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMin();
      break;
    default:
      break;
  }
}

int RunMode::get_mode_type() const {
  return MODE_TYPE_RUN;
}

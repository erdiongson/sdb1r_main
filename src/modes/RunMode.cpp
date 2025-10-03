#include "RunMode.h"
#include "ModesCommon.h"

RunMode::RunMode(DispenserHead& head, Gpu_Hal_Context_t* host, ModeController* controller, ModeCompletionCallback callback)
  : BaseMode(head, host, controller, callback) {}

void RunMode::on_start(Profile& profile) {
  Serial.println("MODE: Setting profile on trayhandler");
  trayHandler.load_profile(profile);
  trayHandler.reset();

  Serial.println("MODE: Setting profile");
  this->profile = profile;

  paused = false;

  Home_Menu(phost, RUNMENU);

  start_stage(ZERO_STAGE);
}

void RunMode::on_interaction(const Interaction& interaction) {
  int button = interaction.key_pressed;
  
  if (button == PAUSE) {
    Serial.println("MODE: Paused");
    paused = true;
    dispenserHead.x().stop();
    dispenserHead.y().stop();
    dispenserHead.z().stop();

    Home_Menu(phost, PAUSEMENU);
  }  else if (button == STOP) {
    Serial.println("MODE: Stopped");
    complete_with_next_mode(MODE_TYPE_HOME) ;
  } else if (button == START) {
    Serial.println("MODE: Resumed");
    paused = false;
    Home_Menu(phost, RUNMENU);
    start_stage(stage);
  }
}

int RunMode::on_step() {
  if (paused) return MODE_CONTINUE;

  switch (stage) {
    case ZERO_STAGE:
      if (dispenserHead.is_steppers_complete()) {
        Serial.println("Zero stage all completed!");

        dispenserHead.x().stop();
        dispenserHead.y().stop();
        dispenserHead.z().stop();

        dispenserHead.x().reset();
        dispenserHead.y().reset();
        dispenserHead.z().reset();
        delay(10);

        Serial.print("Current X Position: ");
        Serial.println(dispenserHead.x().getCurrentPosition());
        Serial.print("Current Y Position: ");
        Serial.println(dispenserHead.y().getCurrentPosition());

        start_stage(OFFSET_STAGE);
      }
      break;
    case OFFSET_STAGE:
      if (dispenserHead.is_steppers_complete()) {
        dispenserHead.x().stop();
        dispenserHead.y().stop();
        delay(10);

        Serial.print("Current X Position: ");
        Serial.println(dispenserHead.x().getCurrentPosition());
        Serial.print("Current Y Position: ");
        Serial.println(dispenserHead.y().getCurrentPosition());

        start_stage(LOWER_HEAD_STAGE);
      }
      break;
    case LOWER_HEAD_STAGE:
      if (dispenserHead.is_steppers_complete()) {
        start_stage(START_DISPENSE_STAGE);
      }
      break;
    case START_DISPENSE_STAGE:
      if (dispenserHead.get_state() == DispenserHead::SENT) {
        start_stage(WAIT_DISPENSE_STAGE);
      }
      break;
    case WAIT_DISPENSE_STAGE:
      if (dispenserHead.get_state() == DispenserHead::COMPLETED) {
        start_stage(RAISE_HEAD_STAGE);
      }
      break;
    case RAISE_HEAD_STAGE:
      if (dispenserHead.is_steppers_complete()) {
        TrayHandler::PositionResult result = trayHandler.goToNextValidPosition();
        if (result.hasNext) {
          target_x = (profile.trayOriginX + (result.position.x * profile.pitch_x)) * -STEPS_PER_UNIT_X;
          target_y = (profile.trayOriginY + (result.position.y * profile.pitch_y)) * STEPS_PER_UNIT_Y;
          start_stage(MOVE_STAGE);
        } else {
          start_stage(HOME_STAGE);
        }
      }
      break;
    case MOVE_STAGE:
      if (dispenserHead.is_steppers_complete()) {
        start_stage(LOWER_HEAD_STAGE);
      }
      break;
    case HOME_STAGE:
      if (dispenserHead.is_steppers_complete()) {
        stage = IDLE_STAGE;
        return MODE_COMPLETE;
      }
      break;
    default:
      break;
  }
  return MODE_CONTINUE;
}

void RunMode::start_stage(int newStage) {
  switch (newStage) {
    case ZERO_STAGE:
      Serial.println("MODE: Setting stage: ZERO");
      this->stage = ZERO_STAGE;

      // TODO: Find out why this is required
      dispenserHead.x().stop();
      dispenserHead.x().reset();
      delay(10);

      dispenserHead.x().moveToMax();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMin();
      break;
    case OFFSET_STAGE:
      Serial.println("MODE: Setting stage: OFFSET");
      this->stage = OFFSET_STAGE;

      // TODO: Find out why this is required
      dispenserHead.x().stop();
      dispenserHead.x().reset();
      delay(10);

      dispenserHead.x().moveTo(STEPS_PER_UNIT_X * -profile.trayOriginX);
      dispenserHead.y().moveTo(STEPS_PER_UNIT_Y * profile.trayOriginY);
      break;
    case LOWER_HEAD_STAGE:
      Serial.println("MODE: Setting stage: LOWER_HEAD");
      this->stage = LOWER_HEAD_STAGE;
      dispenserHead.z().moveTo(STEPS_PER_UNIT_Z * profile.ZDip);
      break;
    case START_DISPENSE_STAGE:
      Serial.println("MODE: Setting stage: START_DISPENSE");
      this->stage = START_DISPENSE_STAGE;
      dispenserHead.send_dispense();
      start_stage(WAIT_DISPENSE_STAGE);
      break;
    case WAIT_DISPENSE_STAGE:
      Serial.println("MODE: Setting stage: WAIT_DISPENSE");
      this->stage = WAIT_DISPENSE_STAGE;
      break;
    case RAISE_HEAD_STAGE:
      Serial.println("MODE: Setting stage: RAISE_HEAD");
      this->stage = RAISE_HEAD_STAGE;
      dispenserHead.z().moveTo(0);
      break;
    case MOVE_STAGE:
      Serial.println("MODE: Setting stage: MOVE");
      this->stage = MOVE_STAGE;

      Serial.println("Setting target x and target y!");
      dispenserHead.x().moveTo( target_x);
      dispenserHead.y().moveTo(target_y);
      break;
    case HOME_STAGE:
      Serial.println("MODE: Setting stage: HOME");
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

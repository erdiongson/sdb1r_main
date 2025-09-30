#include "DispenseMode.h"
#include "../../ui/Platform.h"

DispenseMode::DispenseMode(DispenserHead& head)
  : BaseMode(head) {}

void DispenseMode::on_start(Profile& profile) {
  Serial.println("MODE: Setting profile on trayhandler");
  trayHandler.load_profile(profile);
  trayHandler.reset();

  Serial.println("MODE: Setting profile");
  this->profile = profile;

  paused = false;

  start_stage(ZERO_STAGE);
}

void DispenseMode::on_button_pressed(int button) {
  if (button == PAUSE) {
    Serial.println("MODE: Paused");
    paused = true;
    dispenserHead.x().stop();
    dispenserHead.y().stop();
    dispenserHead.z().stop();
  } else if (button == START) {
    Serial.println("MODE: Resumed");
    paused = false;
    start_stage(stage);
  }
}

int DispenseMode::on_step() {
  if (paused) return MODE_CONTINUE;

  switch (stage) {
    case ZERO_STAGE:
      if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete() && dispenserHead.z().isComplete()) {
        dispenserHead.x().reset();
        dispenserHead.y().reset();
        dispenserHead.z().reset();
        start_stage(OFFSET_STAGE);
      }
      break;
    case OFFSET_STAGE:
      if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete()) {
        start_stage(LOWER_HEAD_STAGE);
      }
      break;
    case LOWER_HEAD_STAGE:
      if (dispenserHead.z().isComplete()) {
        start_stage(START_DISPENSE_STAGE);
      }
      break;
    case START_DISPENSE_STAGE:
      if (dispenserHead.get_state() == DispenserHead::COMPLETED) {
        dispenserHead.send_dispense(1);
        start_stage(WAIT_DISPENSE_STAGE);
      }
      break;
    case WAIT_DISPENSE_STAGE:
      if (dispenserHead.get_state() == DispenserHead::COMPLETED) {
        start_stage(RAISE_HEAD_STAGE);
      }
      break;
    case RAISE_HEAD_STAGE:
      if (dispenserHead.z().isComplete()) {
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
      if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete()) {
        start_stage(HOME_STAGE);
      }
      break;
    case HOME_STAGE:
      if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete() && dispenserHead.z().isComplete()) {
        stage = IDLE_STAGE;
        return MODE_COMPLETE;
      }
      break;
    default:
      break;
  }
  return MODE_CONTINUE;
}

void DispenseMode::start_stage(int newStage) {
  Serial.println("MODE: Setting stage: " + String(newStage));
  switch (newStage) {
    case ZERO_STAGE:
      this->stage = ZERO_STAGE;
      dispenserHead.x().moveToMin();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMin();
      break;
    case OFFSET_STAGE:
      this->stage = OFFSET_STAGE;
      dispenserHead.x().moveTo(STEPS_PER_UNIT_X * profile.trayOriginX);
      dispenserHead.y().moveTo(STEPS_PER_UNIT_Y * profile.trayOriginY);
      break;
    case LOWER_HEAD_STAGE:
      this->stage = LOWER_HEAD_STAGE;
      dispenserHead.z().moveTo(STEPS_PER_UNIT_Z * profile.ZDip);
      break;
    case START_DISPENSE_STAGE:
      this->stage = START_DISPENSE_STAGE;
      dispenserHead.send_dispense(1);
      break;
    case WAIT_DISPENSE_STAGE:
      this->stage = WAIT_DISPENSE_STAGE;
      break;
    case RAISE_HEAD_STAGE:
      this->stage = RAISE_HEAD_STAGE;
      dispenserHead.z().moveTo(0);
      break;
    case MOVE_STAGE:
      this->stage = MOVE_STAGE;
      dispenserHead.x().moveTo(STEPS_PER_UNIT_X * target_x);
      dispenserHead.y().moveTo(STEPS_PER_UNIT_Y * target_y);
      break;
    case HOME_STAGE:
      this->stage = HOME_STAGE;
      dispenserHead.x().moveToMax();
      dispenserHead.y().moveToMin();
      dispenserHead.z().moveToMin();
      break;
    default:
      break;
  }
}

int DispenseMode::get_mode_type() const {
  return MODE_TYPE_DISPENSE;
}

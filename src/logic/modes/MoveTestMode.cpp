#include "MoveTestMode.h"
#include "../../ui/Move_Test_Screen.h"

MoveTestMode::MoveTestMode(DispenserHead& head)
  : BaseMode(head) {}

void MoveTestMode::on_start(Profile& profile) {
  Serial.println("MODE: Move test mode");
  dispenserHead.x().moveToMax();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMin();
}

void MoveTestMode::on_button_pressed(int button) {
  if (button == BACK) {
    back = true;
  }
  if (button == TAG_MOVE_UP) {
    Serial.println("MODE: Move up");
    dispenserHead.y().moveBy(STEPS_PER_UNIT_Y * 5 * 100);
  }
  if (button == TAG_MOVE_DOWN) {
    Serial.println("MODE: Move down");
    dispenserHead.y().moveBy(-STEPS_PER_UNIT_Y * 5 * 100);
  }
  if (button == TAG_MOVE_LEFT) {
    Serial.println("MODE: Move left");
    dispenserHead.x().moveBy(STEPS_PER_UNIT_X * 5 * 100);
  }
  if (button == TAG_MOVE_RIGHT) {
    Serial.println("MODE: Move right");
    dispenserHead.x().moveBy(-STEPS_PER_UNIT_X * 5 * 100);
  }
  if (button == TAG_Z_UP) {
    Serial.println("MODE: Move z up");
    dispenserHead.z().moveBy(-STEPS_PER_UNIT_Z * 1 * 100);
  }
  if (button == TAG_Z_DOWN) {
    Serial.println("MODE: Move z down");
    dispenserHead.z().moveBy(STEPS_PER_UNIT_Z * 1 * 100);
  }
}

int MoveTestMode::on_step() {
  if (back) {
    return MODE_COMPLETE;
  }
  return MODE_CONTINUE;
}

int MoveTestMode::get_mode_type() const {
  return MODE_TYPE_MOVE_TEST;
}

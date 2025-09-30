#include "HomeMode.h"

HomeMode::HomeMode(DispenserHead& head)
  : BaseMode(head) {}

void HomeMode::on_start(Profile& profile) {
  Serial.println("MODE: Home mode");
  dispenserHead.x().moveToMax();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMin();
}

void HomeMode::on_button_pressed(int button) {}

int HomeMode::on_step() {
  if (dispenserHead.x().isComplete() && dispenserHead.y().isComplete() && dispenserHead.z().isComplete()) {
    Serial.println("MODE: Home mode complete");
    return MODE_COMPLETE;
  }
  return MODE_CONTINUE;
}

int HomeMode::get_mode_type() const {
  return MODE_TYPE_HOME;
}
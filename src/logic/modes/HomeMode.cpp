#include "HomeMode.h"
#include "../../ui/App_Common.h"

HomeMode::HomeMode(DispenserHead& head, Gpu_Hal_Context_t *host)
  : BaseMode(head, host) {}

void HomeMode::on_start(Profile& profile) {
  Serial.println("MODE: Home mode");

  Home_Menu(&host, MAINMENU);

  dispenserHead.x().moveToMax();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMin();
}

void HomeMode::on_interaction(const Interaction& interaction) {}

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
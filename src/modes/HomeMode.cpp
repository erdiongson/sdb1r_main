#include "HomeMode.h"
#include "BaseMode.h"
#include "ModesCommon.h"

HomeMode::HomeMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback)
  : BaseMode(head, host, controller, callback) {}

void HomeMode::on_start(Profile& profile) {
  Serial.println("MODE: Home mode");

  Home_Menu(&host, MAINMENU);

  dispenserHead.x().moveToMax();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMin();
}

void HomeMode::on_interaction(const Interaction& interaction) {
  if (interaction.key_pressed == SETTING) {
    Serial.println("MODE: Home mode transitioning to settings");
    complete_with_next_mode(MODE_TYPE_CONFIG);
  } else if (interaction.key_pressed == START) {
    Serial.println("MODE: Home mode transitioning to start");
    complete_with_next_mode(MODE_TYPE_RUN);
  }
}

int HomeMode::on_step() {
  return MODE_CONTINUE;
}

int HomeMode::get_mode_type() const {
  return MODE_TYPE_HOME;
}
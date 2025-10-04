#include "HomeMode.h"
#include "BaseMode.h"
#include "ModesCommon.h"
#include "../views/Home_Screen.h"

HomeMode::HomeMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback)
  : BaseMode(head, host, controller, callback) {}

void HomeMode::on_start(Profile& profile) {
  Serial.println("MODE: Home mode");

  Home_Screen(&host, MAINMENU);

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
  } else if (interaction.plc_message_type == MSG_START) {
    Serial.println("MODE: Home mode transitioning to start");
    complete_with_next_mode(MODE_TYPE_RUN);
  } else if (interaction.plc_message_type == MSG_RAISE_Z) {
    Serial.println("MODE: Home mode raising Z");
    dispenserHead.z().moveBy(-interaction.plc_message_data * STEPS_PER_UNIT_Z);
  } else if (interaction.plc_message_type == MSG_LOWER_Z) {
    Serial.println("MODE: Home mode lowering Z");
    dispenserHead.z().moveBy(interaction.plc_message_data * STEPS_PER_UNIT_Z);
  }
}

ModeStepResult HomeMode::on_step() {
  DispenserProcessResult result = dispenserHead.process();
  return ModeStepResult(result.steppers, result.dispenser);
}

int HomeMode::get_mode_type() const {
  return MODE_TYPE_HOME;
}
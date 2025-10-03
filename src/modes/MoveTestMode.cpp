#include "MoveTestMode.h"
#include "../views/Move_Test_Screen.h"

MoveTestMode::MoveTestMode(DispenserHead& head, Gpu_Hal_Context_t *host, ModeController* controller, ModeCompletionCallback callback)
  : BaseMode(head, host, controller, callback) {}

void MoveTestMode::on_start(Profile& profile) {
  Serial.println("MODE: Move test mode");

  Move_Test_Screen(phost, {});  
}

void MoveTestMode::on_interaction(const Interaction& interaction) {
  int button = interaction.key_pressed;
  
  if (button == TAG_MOVE_UP) {
    Serial.println("MODE: Move up");
    dispenserHead.y().moveBy(STEPS_PER_UNIT_Y * 1 * 30);
    // dispenserHead.y().runUntilCompleteBlocking();
  }
  if (button == TAG_MOVE_DOWN) {
    Serial.println("MODE: Move down");
    dispenserHead.y().moveBy(-STEPS_PER_UNIT_Y * 1 * 30);
    // dispenserHead.y().runUntilCompleteBlocking();
  }
  if (button == TAG_MOVE_LEFT) {
    Serial.println("MODE: Move left");
    dispenserHead.x().moveBy(STEPS_PER_UNIT_X * 1 * 30);
  }
  if (button == TAG_MOVE_RIGHT) {
    Serial.println("MODE: Move right");
    dispenserHead.x().moveBy(-STEPS_PER_UNIT_X * 1 * 30);
  }
  if (button == TAG_Z_UP) {
    Serial.println("MODE: Move z up");
    dispenserHead.z().moveBy(-STEPS_PER_UNIT_Z * 1 * 100);
  }
  if (button == TAG_Z_DOWN) {
    Serial.println("MODE: Move z down");
    dispenserHead.z().moveBy(STEPS_PER_UNIT_Z * 1 * 100);
  }
  if (button == TAG_MOVE_BACK) {
    Serial.println("MODE: Move back");
    complete_with_next_mode(MODE_TYPE_HOME);
  }
}

ModeStepResult MoveTestMode::on_step() {
  DispenserProcessResult result = dispenserHead.process();
  
  if (result.steppers == AXIS_STATE_COMPLETE) {
    LimitSwitchStates limitStates;
    limitStates.x_max_limit = dispenserHead.x().isAtMax();
    limitStates.x_min_limit = dispenserHead.x().isAtMin();
    limitStates.y_max_limit = dispenserHead.y().isAtMax();
    limitStates.y_min_limit = dispenserHead.y().isAtMin();
    limitStates.z_max_limit = dispenserHead.z().isAtMax();
    limitStates.z_min_limit = dispenserHead.z().isAtMin();
    Move_Test_Screen(phost, limitStates);
  }

  return ModeStepResult(result.steppers, result.dispenser);
}

int MoveTestMode::get_mode_type() const {
  return MODE_TYPE_MOVE_TEST;
}

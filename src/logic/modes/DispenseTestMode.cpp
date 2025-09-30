#include "DispenseTestMode.h"
#include "../../ui/Dispense_Test_Screen.h"

DispenseTestMode::DispenseTestMode(DispenserHead& head, Gpu_Hal_Context_t *host)
  : BaseMode(head, host) {}

void DispenseTestMode::on_start(Profile& profile) {
  Serial.println("MODE: Dispense test mode");
  
  // Initialize UI
  DispenseTestSettings settings = {0, 0};
  Dispense_Test_Screen(phost, settings);
  
  // Initialize to home position
  dispenserHead.x().moveToMax();
  dispenserHead.y().moveToMin();
  dispenserHead.z().moveToMin();
}

void DispenseTestMode::on_interaction(const Interaction& interaction) {
  int button = interaction.key_pressed;
  
  if (button == TAG_DISPENSE_BACK) {
    back = true;
  }
  
  // Dispense button
  if (button == TAG_DISPENSE) {
    Serial.println("MODE: Dispense test - setting vibration and dispensing");
    // Set vibration level and time, then dispense
    if (dispenserHead.set_vibration_level(current_vibration_level)) {
      Serial.println("MODE: Vibration level set to " + String(current_vibration_level));
      if (dispenserHead.set_vibration_time(current_vibration_time)) {
        Serial.println("MODE: Vibration time set to " + String(current_vibration_time) + "s");
        dispenserHead.send_dispense();
      }
    }
  }
  
  // Vibration level buttons (U0-U4)
  if (button == TAG_VIB_U0) {
    current_vibration_level = 0;
    Serial.println("MODE: Vibration level set to U0");
  }
  if (button == TAG_VIB_U1) {
    current_vibration_level = 1;
    Serial.println("MODE: Vibration level set to U1");
  }
  if (button == TAG_VIB_U2) {
    current_vibration_level = 2;
    Serial.println("MODE: Vibration level set to U2");
  }
  if (button == TAG_VIB_U3) {
    current_vibration_level = 3;
    Serial.println("MODE: Vibration level set to U3");
  }
  if (button == TAG_VIB_U4) {
    current_vibration_level = 4;
    Serial.println("MODE: Vibration level set to U4");
  }
  
  // Vibration time buttons (1-5s)
  if (button == TAG_VIB_TIME_1) {
    current_vibration_time = 1;
    Serial.println("MODE: Vibration time set to 1s");
  }
  if (button == TAG_VIB_TIME_2) {
    current_vibration_time = 2;
    Serial.println("MODE: Vibration time set to 2s");
  }
  if (button == TAG_VIB_TIME_3) {
    current_vibration_time = 3;
    Serial.println("MODE: Vibration time set to 3s");
  }
  if (button == TAG_VIB_TIME_4) {
    current_vibration_time = 4;
    Serial.println("MODE: Vibration time set to 4s");
  }
  if (button == TAG_VIB_TIME_5) {
    current_vibration_time = 5;
    Serial.println("MODE: Vibration time set to 5s");
  }
}

int DispenseTestMode::on_step() {
  if (back) {
    return MODE_COMPLETE;
  }
  return MODE_CONTINUE;
}

int DispenseTestMode::get_mode_type() const {
  return MODE_TYPE_DISPENSE_TEST;
}

uint8_t DispenseTestMode::get_current_vibration_level() const {
  return current_vibration_level;
}

uint8_t DispenseTestMode::get_current_vibration_time() const {
  return current_vibration_time;
}

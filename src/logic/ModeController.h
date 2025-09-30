#ifndef MODE_CONTROLLER_H
#define MODE_CONTROLLER_H

#include "DispenserHead.h"
#include "../../SaveProfile.h"
#include "../ui/App_Common.h"

#include "modes/BaseMode.h"
#include "modes/HomeMode.h"
#include "modes/DispenseMode.h"
#include "modes/MoveTestMode.h"
#include "modes/DispenseTestMode.h"

class ModeController {
private:
  BaseMode* mode;
  DispenserHead& dispenserHead;

public:
  // Constructor that takes a reference to the dispenser head
  ModeController(DispenserHead& head)
    : dispenserHead(head), mode(nullptr) {}

  // Destructor to clean up current mode
  ~ModeController() {
    if (mode != nullptr) {
      delete mode;
    }
  }

  // Call the current mode's on_step function
  int on_step() {
    if (mode != nullptr) {
      return mode->on_step();
    }
    return MODE_CONTINUE;  // No mode active
  }

  void on_button_pressed(int button) {
    if (mode != nullptr) {
      mode->on_button_pressed(button);
    }
  }

  // Set a new mode (deletes previous mode to save memory)
  void start_mode(int modeType, Profile& profile, Gpu_Hal_Context_t *phost) {
    if (mode != nullptr) {
      delete mode;
    }
    switch(modeType) {
      case MODE_TYPE_HOME:
        mode = new HomeMode(dispenserHead, phost);
        break;
      case MODE_TYPE_DISPENSE:
        mode = new DispenseMode(dispenserHead, phost);
        break;
      case MODE_TYPE_MOVE_TEST:
        mode = new MoveTestMode(dispenserHead, phost);
        break;
      case MODE_TYPE_DISPENSE_TEST:
        mode = new DispenseTestMode(dispenserHead, phost);
        break;
    }
    mode->on_start(profile);
  }

  // Get current mode (can be nullptr)
  BaseMode* getCurrentMode() {
    return mode;
  }

  // Check if a mode is currently active
  bool hasActiveMode() {
    return mode != nullptr;
  }
};

#endif  // MODE_CONTROLLER_H
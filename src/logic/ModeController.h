#ifndef MODE_CONTROLLER_H
#define MODE_CONTROLLER_H

#include "DispenserHead.h"
#include "Modes.h"
#include "../../SaveProfile.h"

class ModeController {
private:
  Mode* mode;
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
  template<typename ModeType>
  void start_mode(Profile& profile) {
    if (mode != nullptr) {
      delete mode;
    }
    mode = new ModeType(dispenserHead);
    mode->on_start(profile);
  }

  // Get current mode (can be nullptr)
  Mode* getCurrentMode() {
    return mode;
  }

  // Check if a mode is currently active
  bool hasActiveMode() {
    return mode != nullptr;
  }
};

#endif  // MODE_CONTROLLER_H
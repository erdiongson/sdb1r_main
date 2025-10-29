#pragma once

#include "../../../Config.h"
#include "../BaseController.h"

// Repeat dispense state structure.
struct RepeatDispenseState {
  bool enabled;
  int current_count;
  int total_count;
};

class DispenseTestController : public BaseController {
 private:
  enum State {
    READY,
    WAITING_FOR_RESPONSE,
    RECEIVED_RESPONSE,
  };

  State state;
  int repeat_count = DEFAULT_DISPENSE_REPEAT_COUNT;  // Number of times to repeat dispense
  RepeatDispenseState repeat_state = {false, 0, 0};         // Repeat state tracker
  char status_message_buffer[50];  // Buffer for status messages that need to persist

  // Updates the screen with current state.
  void updateScreen(const char* status_message);

 public:
  DispenseTestController(ControllerParams params);
  void onStart() override;
  void onInteraction(const Interaction& interaction) override;
  ControllerStepResult onStep() override;

  int getModeType() const override;
};
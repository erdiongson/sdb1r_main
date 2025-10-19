#pragma once

#include "../serial/PlcSerial.h"
#include "../gpu/Platform.h"
#include "../gpu/App_Common.h"
#include "../Utils.h"

struct Interaction {
  int key_pressed;                  // Key code if a key was pressed, 0 otherwise
  PLCMessageType plc_message_type;  // Type of PLC message received, MSG_UNKNOWN if none
  byte plc_message_data;            // Data value from PLC message, 0 if none
};

class InteractionsHandler {
 public:
  InteractionsHandler() {}
  ~InteractionsHandler() {}

  static inline int last_touch_state = 0;  // Inline static member (C++17)

  static int waitForTouchRelease() {
    while (Gpu_Hal_Rd8(phost, REG_TOUCH_TAG) != 0) {
      delay(10);
    }
  };

  static int getTouchStateChanged() {
    // Read hardware register only once
    int latestTouchState = Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);

    // Check if the state changed
    if (last_touch_state != latestTouchState) {
      last_touch_state = latestTouchState;
      return latestTouchState;
    }

    // Update last button state for next check
    last_touch_state = latestTouchState;
    return -1;
  };

  // Alternative: even faster version that returns early when no interaction
  static bool getAllInteractions(Interaction& interaction) {
    // Read hardware register only once
    int touchButtonPressed = Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);

    // Check for button release (the only case we care about)
    if (last_touch_state == 0 && touchButtonPressed != 0) {
      // Button was pressed and now released - register the press
      interaction.key_pressed = touchButtonPressed;
      interaction.plc_message_type = MSG_UNKNOWN;
      interaction.plc_message_data = 0;
      last_touch_state = touchButtonPressed;
      return true;  // Interaction detected
    }

    // Update last button state
    last_touch_state = touchButtonPressed;

    // Check for PLC messages before early exit
    PLCMessage plcMessage = PlcSerial::process();
    if (plcMessage.type != MSG_UNKNOWN) {
      snprintf(g_log_buffer, sizeof(g_log_buffer), "PLC message received: %d", plcMessage.type);
      Logger::log(g_log_buffer);
      interaction.key_pressed = 0;
      interaction.plc_message_type = plcMessage.type;
      interaction.plc_message_data = plcMessage.data_value;
      return true;  // PLC interaction detected
    }

    // No interaction detected - early exit without initializing struct
    return false;
  }
};

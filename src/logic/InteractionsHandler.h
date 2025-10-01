#pragma once

#include "../communication/PlcSerial.h"
#include "../gpu/Platform.h"
#include "../gpu/App_Common.h"

struct Interaction {
    int key_pressed;            // Key code if a key was pressed, 0 otherwise
    PLCMessageType plc_message_type; // Type of PLC message received, MSG_UNKNOWN if none
};

class InteractionsHandler {
public:
    InteractionsHandler() {}
    ~InteractionsHandler() {}

    Interaction getInteraction() {
        Interaction interaction;
        
        // Initialize with default values
        interaction.key_pressed = 0;
        interaction.plc_message_type = MSG_UNKNOWN;
        
        // Check for key presses - only register a key when it's pressed and then released
        static int lastTouchButton = 0; // Remember the last button state
        int touchButtonPressed = GetKeyPressed();
        
        // Button press cycle: non-zero (pressed) followed by zero (released)
        if (lastTouchButton != 0 && touchButtonPressed == 0) {
            // Button was pressed and now released - register the press
            interaction.key_pressed = lastTouchButton;
        }
        
        // Update last button state for next check
        lastTouchButton = touchButtonPressed;
        
        // Check for PLC messages
        PLCMessage plcMessage = PlcSerial::getNewMessage();
        interaction.plc_message_type = plcMessage.type;
        
        return interaction;
    }
};

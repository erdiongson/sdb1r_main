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

    static inline int lastTouchButton = 0; // Inline static member (C++17)

    // Optimized version: pass by reference to avoid struct copy
    void getInteraction(Interaction& interaction) {
        // Initialize with default values using member initialization
        interaction = {0, MSG_UNKNOWN};
        
        // Read hardware register only once
        int touchButtonPressed = Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);
        
        // Button press cycle: non-zero (pressed) followed by zero (released)
        if (lastTouchButton != 0 && touchButtonPressed == 0) {
            // Button was pressed and now released - register the press
            interaction.key_pressed = lastTouchButton;
        }
        
        // Update last button state for next check
        lastTouchButton = touchButtonPressed;
        
        // Check for PLC messages
        // PLCMessage plcMessage = PlcSerial::getNewMessage();
        // interaction.plc_message_type = plcMessage.type;
    }
    
    // Alternative: even faster version that returns early when no interaction
    bool getInteractionFast(Interaction& interaction) {
        // Read hardware register only once
        int touchButtonPressed = Gpu_Hal_Rd8(phost, REG_TOUCH_TAG);
        
        // Check for button release (the only case we care about)
        if (lastTouchButton != 0 && touchButtonPressed == 0) {
            // Button was pressed and now released - register the press
            interaction.key_pressed = lastTouchButton;
            interaction.plc_message_type = MSG_UNKNOWN;
            lastTouchButton = touchButtonPressed;
            return true; // Interaction detected
        }
        
        // Update last button state
        lastTouchButton = touchButtonPressed;
        
        // No interaction detected - early exit without initializing struct
        return false;
    }
};

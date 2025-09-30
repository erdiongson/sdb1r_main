#pragma once

#include "../communication/PlcSerial.h"

/**
 * @brief Structure to hold interaction information from various inputs
 */
struct Interaction {
    int key_pressed;            // Key code if a key was pressed, 0 otherwise
    PLCMessageType plc_message_type; // Type of PLC message received, MSG_UNKNOWN if none
};

/**
 * @brief Handles user and system interactions from various input sources
 */
class InteractionsHandler {
public:
    InteractionsHandler();
    ~InteractionsHandler();

    /**
     * @brief Process all pending interactions
     */
    void handleInteractions();
    
    /**
     * @brief Get the latest interaction from inputs
     * @return Interaction struct containing key press and PLC message information
     */
    Interaction getInteraction();
};

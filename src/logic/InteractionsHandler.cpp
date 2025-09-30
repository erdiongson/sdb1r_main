/* 
 * InteractionsHandler.cpp
 * Implementation of the InteractionsHandler class for managing user and system interactions
 */

#include "InteractionsHandler.h"
#include "../communication/PlcSerial.h"
#include "../ui/Platform.h"
#include "../ui/App_Common.h"

/**********************************************************************************************************
* @brief InteractionsHandler::InteractionsHandler()
* @details Constructor for InteractionsHandler class
**********************************************************************************************************/
InteractionsHandler::InteractionsHandler() {
    // Initialize any required resources
}

/**********************************************************************************************************
* @brief InteractionsHandler::~InteractionsHandler()
* @details Destructor for InteractionsHandler class
**********************************************************************************************************/
InteractionsHandler::~InteractionsHandler() {
    // Clean up any resources
}

/**********************************************************************************************************
* @brief InteractionsHandler::handleInteractions()
* @details Process all pending interactions from various input sources
**********************************************************************************************************/
void InteractionsHandler::handleInteractions() {
    // Get the latest interaction
    Interaction interaction = getInteraction();
    
    // Process the interaction based on its type
    if (interaction.key_pressed != 0) {
        // Handle key press
        // Example: Serial.println("Key pressed: " + String(interaction.key_pressed));
    }
    
    if (interaction.plc_message_type != MSG_UNKNOWN) {
        // Handle PLC message
        // Example: Serial.println("PLC message: " + String(PlcSerial::getMessageTypeName(interaction.plc_message_type)));
    }
}

/**********************************************************************************************************
* @brief InteractionsHandler::getInteraction()
* @details Get the latest interaction from inputs (keyboard, PLC, etc.)
* @return Interaction struct containing key press and PLC message information
**********************************************************************************************************/
Interaction InteractionsHandler::getInteraction() {
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

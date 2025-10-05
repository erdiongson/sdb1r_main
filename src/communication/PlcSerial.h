/* Author : Ryan Y.
 * Date created - 2024.08.27 - PLC Message Parsing Module
 * Created for: Message type identification and data extraction
 * Modified - 2025.09.30 - Converted to class-based implementation
 */

#pragma once

#include <Arduino.h>



#define START_BYTE 0xEF
#define END_BYTE 0xFE
#define PLC_MESSAGE_LENGTH 5

enum PLCMessageType {
  MSG_UNKNOWN = 0,
  MSG_START = 1,
  MSG_STOP = 2,
  MSG_PAUSE = 3,
  MSG_RAISE_Z = 4,
  MSG_LOWER_Z = 5
};

// Structure to hold parsed message information
struct PLCMessage {
  PLCMessageType type;
  byte data[5];  // Raw message data
  byte dataValue; // Extracted data value (for messages with data)
};

// PlcSerial class for handling PLC communication.
class PlcSerial {
public:
  // Process incoming data from the PLC.
  // @return PLCMessage struct containing message type and data.
  static PLCMessage process();
};

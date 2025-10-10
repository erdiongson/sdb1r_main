/* Author : Ryan Y.
 * Date created - 2024.08.27 - PLC Message Parsing Module
 * Created for: Message type identification and data extraction
 * Modified - 2025.09.30 - Converted to class-based implementation
 */

#include "PlcSerial.h"

// Process incoming data from the PLC.
// @return PLCMessage struct containing message type and data.
PLCMessage PlcSerial::process() {
  PLCMessage result;
  result.type = MSG_UNKNOWN;
  result.dataValue = 0;
  
  if (Serial3.available() == 0) return result;
  
  // Check if first byte is START_BYTE
  if (Serial3.peek() != START_BYTE) {
    Serial3.read(); // Discard invalid byte
    return result;
  }
  
  // Check for complete message
  if (Serial3.available() < PLC_MESSAGE_LENGTH) return result;
  
  uint8_t response[PLC_MESSAGE_LENGTH];
  Serial3.readBytes(response, PLC_MESSAGE_LENGTH);
  
  // Validate the response format
  if (response[0] != START_BYTE || response[4] != END_BYTE) {
    return result;
  }
  
  // Validate checksum (byte [3] should equal sum of byte [1] and [2])
  if (response[3] != ((response[1] + response[2]) & 0xFF)) {
    return result;
  }
  
  // Copy raw data
  for (int i = 0; i < PLC_MESSAGE_LENGTH; i++) {
    result.data[i] = response[i];
  }
  
  // Store data value
  result.dataValue = response[2];
  
  // Determine message type based on command byte [1]
  switch (response[1]) {
    case 0x30: result.type = MSG_START; break;
    case 0x31: result.type = MSG_STOP; break;
    case 0x32: result.type = MSG_PAUSE; break;
    case 0x42: result.type = MSG_RAISE_Z; break;
    case 0x41: result.type = MSG_LOWER_Z; break;
    default: result.type = MSG_UNKNOWN; break;
  }
  
  return result;
}
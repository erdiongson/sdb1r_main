/* Author : Ryan Y.
 * Date created - 2024.08.27 - PLC Message Parsing Module
 * Created for: Message type identification and data extraction
 * Modified - 2025.09.30 - Converted to class-based implementation
 */

#include "PlcSerial.h"

// Initialize static member
bool PlcSerial::isBusy = false;

// Process incoming data from the PLC.
// @return PLCMessage struct containing message type and data.
PLCMessage PlcSerial::process() {
  PLCMessage result;
  result.type = MSG_UNKNOWN;
  result.data_value = 0;

  if (Serial3.available() == 0) return result;

  // Check if first byte is START_BYTE
  if (Serial3.peek() != START_BYTE) {
    Serial3.read();  // Discard invalid byte
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

  // Check if this is a START command and system is busy
  if (response[1] == PLC_CMD_START && isBusy) {
    // Send 5 NAK bytes instead of mirroring
    uint8_t nakResponse[5] = { PLC_NAK_BYTE, PLC_NAK_BYTE, PLC_NAK_BYTE, PLC_NAK_BYTE, PLC_NAK_BYTE };
    Serial3.write(nakResponse, 5);
  } else {
    // Send acknowledgement (mirror the received message back to PLC)
    Serial3.write(response, PLC_MESSAGE_LENGTH);
  }

  // Store data value
  result.data_value = response[2];

  // Determine message type based on command byte [1]
  switch (response[1]) {
    case PLC_CMD_START:
      result.type = MSG_START;
      break;
    case PLC_CMD_STOP:
      result.type = MSG_STOP;
      break;
    case PLC_CMD_PAUSE:
      result.type = MSG_PAUSE;
      break;
    case PLC_CMD_RAISE_Z:
      result.type = MSG_RAISE_Z;
      break;
    case PLC_CMD_LOWER_Z:
      result.type = MSG_LOWER_Z;
      break;
    default:
      result.type = MSG_UNKNOWN;
      break;
  }

  return result;
}

// Send a "COMPLETED" message to the PLC.
// Message format: EF 17 00 17 FE
void PlcSerial::sendCompleted() {
  uint8_t message[PLC_MESSAGE_LENGTH] = {
    START_BYTE,  // 0xEF
    0x17,        // Command byte
    0x00,        // Data byte
    0x17,        // Checksum (0x17 + 0x00 = 0x17)
    END_BYTE     // 0xFE
  };
  Serial3.write(message, PLC_MESSAGE_LENGTH);
}

// Set the busy state.
// @param busy True if the system is busy, false otherwise.
void PlcSerial::setBusy(bool busy) {
  isBusy = busy;
}
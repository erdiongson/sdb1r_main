/* Author : Ryan Y.
 * Date created - 2024.08.27 - PLC Message Parsing Module
 * Created for: Message type identification and data extraction
 * Modified - 2025.09.30 - Converted to class-based implementation
 */

#include "PlcSerial.h"

// Parse received PLC message and identify message type with data extraction.
// @param receivedData Pointer to received message bytes.
// @param length Length of received message.
// @return PLCMessage struct containing message type and data.
PLCMessage PlcSerial::parseReceivedMessage(const byte *receivedData, int length) {
  PLCMessage result;
  result.type = MSG_UNKNOWN;
  result.dataValue = 0;

  // Copy raw data
  for (int i = 0; i < 5 && i < length; i++) {
    result.data[i] = receivedData[i];
  }

  // Check if message length is correct
  if (length != PLC_MESSAGE_LENGTH) {
    return result;
  }

  // Check if start and end bytes are correct
  if (receivedData[0] != START_BYTE || receivedData[4] != END_BYTE) {
    return result;
  }

  // Validate checksum (byte [3] should equal sum of byte [1] and [2])
  if (receivedData[3] != ((receivedData[1] + receivedData[2]) & 0xFF)) {
    return result; // Invalid checksum
  }

  // Extract data from byte [2]
  result.dataValue = receivedData[2];

  // Determine message type based on command byte [1]
  switch (receivedData[1]) {
  case 0x30:
    result.type = MSG_START;
    break;
  case 0x31:
    result.type = MSG_STOP;
    break;
  case 0x32:
    result.type = MSG_PAUSE;
    break;
  case 0x42:
    result.type = MSG_RAISE_Z;
    break;
  case 0x41:
    result.type = MSG_LOWER_Z;
    break;
  default:
    result.type = MSG_UNKNOWN;
    break;
  }

  return result;
}

// Get string representation of message type for debugging.
// @param type PLCMessageType enum value.
// @return String name of message type.
const char *PlcSerial::getMessageTypeName(PLCMessageType type) {
  switch (type) {
  case MSG_START:
    return "START";
  case MSG_STOP:
    return "STOP";
  case MSG_PAUSE:
    return "PAUSE";
  case MSG_RAISE_Z:
    return "RAISE_Z";
  case MSG_LOWER_Z:
    return "LOWER_Z";
  case MSG_UNKNOWN:
  default:
    return "UNKNOWN";
  }
}

// Check if data is available from PLC and receive it into the provided buffer.
// @param buffer Pointer to buffer to store received data.
// @param bufferSize Size of the buffer.
// @param bytesReceived Pointer to variable to store number of bytes received.
// @return True if data was received, false otherwise.
bool PlcSerial::checkAndReceiveData(byte *buffer, int bufferSize, int *bytesReceived) {
  *bytesReceived = 0;
  
  // Check if data is available
  if (!Serial3.available()) {
    return false;
  }
  
  // Read all available bytes
  while (Serial3.available() && *bytesReceived < bufferSize) {
    buffer[*bytesReceived] = Serial3.read();
    (*bytesReceived)++;
  }
  
  return (*bytesReceived > 0);
}

// Check for new PLC messages, receive data if available, and parse the message.
// @return PLCMessage struct containing message type and data.
PLCMessage PlcSerial::getNewMessage() {
  static byte receiveBuffer[32]; // Buffer to store received data
  int bytesReceived = 0;
  
  // Initialize empty message with unknown type
  PLCMessage message;
  message.type = MSG_UNKNOWN;
  message.dataValue = 0;
  
  // Check for and receive data
  if (checkAndReceiveData(receiveBuffer, sizeof(receiveBuffer), &bytesReceived)) {
    Serial.print("Received data: ");
    for (int i = 0; i < bytesReceived; i++) {
      Serial.print("0x");
      Serial.print(receiveBuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    
    // Parse the received message
    message = parseReceivedMessage(receiveBuffer, bytesReceived);
    
    // Print message type for debugging
    Serial.print("Message type: ");
    Serial.print(getMessageTypeName(message.type));
    Serial.print(", Data value: 0x");
    Serial.println(message.dataValue, HEX);
  }
  
  return message;
}

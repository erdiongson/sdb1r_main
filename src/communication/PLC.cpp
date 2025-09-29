/* Author : Ryan Y.
 * Date created - 2024.08.27 - PLC Message Parsing Module
 * Created for: Message type identification and data extraction
 */

#include "PLC.h"
#include "UART.h"

/**********************************************************************************************************
* @brief parseReceivedMessage()
* @details Parse received PLC message and identify message type with data extraction
* @param receivedData - pointer to received message bytes
* @param length - length of received message
* @return PLCMessage struct containing message type and data
**********************************************************************************************************/
PLCMessage parseReceivedMessage(const byte *receivedData, int length) {
  PLCMessage result;
  result.type = MSG_UNKNOWN;
  result.dataValue = 0;

  // Copy raw data
  for (int i = 0; i < 5 && i < length; i++) {
    result.data[i] = receivedData[i];
  }

  // Check if message length is correct
  if (length != messageLength) {
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

/**********************************************************************************************************
* @brief getMessageTypeName()
* @details Get string representation of message type for debugging
* @param type - PLCMessageType enum value
* @return const char* - string name of message type
**********************************************************************************************************/
const char *getMessageTypeName(PLCMessageType type) {
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

/**********************************************************************************************************
* @brief checkAndReceiveData()
* @details Check if data is available from PLC and receive it into the provided buffer
* @param buffer - pointer to buffer to store received data
* @param bufferSize - size of the buffer
* @param bytesReceived - pointer to variable to store number of bytes received
* @return bool - true if data was received, false otherwise
**********************************************************************************************************/
bool checkAndReceiveData(byte *buffer, int bufferSize, int *bytesReceived) {
  // Check if data is available
  if (!Serial2.available()) {
    return false;
  }
  
  // Read available data into buffer
  *bytesReceived = 0;
  while (Serial2.available() && *bytesReceived < bufferSize) {
    buffer[*bytesReceived] = Serial2.read();
    (*bytesReceived)++;
    
    // If we've received a complete message (indicated by END_BYTE), stop reading
    if (buffer[*bytesReceived - 1] == END_BYTE && *bytesReceived >= messageLength) {
      return true;
    }
  }
  
  // Return true if we received any data
  return (*bytesReceived > 0);
}

/**********************************************************************************************************
* @brief getNewMessage()
* @details Check for new PLC messages, receive data if available, and parse the message
* @return PLCMessage struct containing message type and data
**********************************************************************************************************/
PLCMessage getNewMessage() {
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

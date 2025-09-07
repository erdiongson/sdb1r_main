/* Author : Ryan Y.
 * Date created - 2024.08.27 - PLC Message Parsing Module
 * Created for: Message type identification and data extraction
 */

#include "PLC.h"

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

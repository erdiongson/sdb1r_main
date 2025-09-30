/* Author : Ryan Y.
 * Date created - 2024.08.27 - PLC Message Parsing Module
 * Created for: Message type identification and data extraction
 * Modified - 2025.09.30 - Converted to class-based implementation
 */

#pragma once

#include <stdint.h>

#define START_BYTE 0xEF
#define END_BYTE 0xFE

// Define byte type if not already defined
#ifndef byte
typedef uint8_t byte;
#endif

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

// Shared message byte sequences
static const byte expectedMessageStart[] = {START_BYTE, 0x30, 0x00, 0x30, END_BYTE};
static const byte expectedMessageStop[] = {START_BYTE, 0x31, 0x00, 0x31, END_BYTE};
static const byte expectedMessagePause[] = {START_BYTE, 0x32, 0x00, 0x32, END_BYTE};
static const byte expectedMessageRaiseZ[] = {START_BYTE, 0x42, 0x00, 0x00, END_BYTE};
static const byte expectedMessageLowerZ[] = {START_BYTE, 0x41, 0x00, 0x00, END_BYTE};
static const byte responseMessage[] = {START_BYTE, 0x17, 0x00, 0x17, END_BYTE};
static const int messageLength = 5;

/**
 * @brief PlcSerial class for handling PLC communication
 */
class PlcSerial {
public:
  /**
   * @brief Parse received PLC message and identify message type with data extraction
   * @param receivedData - pointer to received message bytes
   * @param length - length of received message
   * @return PLCMessage struct containing message type and data
   */
  static PLCMessage parseReceivedMessage(const byte* receivedData, int length);
  
  /**
   * @brief Get string representation of message type for debugging
   * @param type - PLCMessageType enum value
   * @return const char* - string name of message type
   */
  static const char* getMessageTypeName(PLCMessageType type);
  
  /**
   * @brief Check if data is available from PLC and receive it into the provided buffer
   * @param buffer - pointer to buffer to store received data
   * @param bufferSize - size of the buffer
   * @param bytesReceived - pointer to variable to store number of bytes received
   * @return bool - true if data was received, false otherwise
   */
  static bool checkAndReceiveData(byte* buffer, int bufferSize, int* bytesReceived);
  
  /**
   * @brief Check for new PLC messages, receive data if available, and parse the message
   * @return PLCMessage struct containing message type and data
   */
  static PLCMessage getNewMessage();
};

/* Author : Ryan Y.
 * Date created - 2024.08.27 - PLC Message Parsing Module
 * Created for: Message type identification and data extraction
 */

#ifndef _PLC_H_
#define _PLC_H_

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

// Function declarations
PLCMessage parseReceivedMessage(const byte* receivedData, int length);
const char* getMessageTypeName(PLCMessageType type);

#endif

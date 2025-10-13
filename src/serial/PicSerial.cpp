#include "PicSerial.h"
#include "../Utils.h"

// Initialize static member.
unsigned long PicSerial::timeout_at = 0;

// Send a message to the dispenser with command and data bytes.
// @param command Command byte to send.
// @param data Data byte to send.
void PicSerial::sendMessage(byte command, byte data) {
  // Clear serial buffer
  while (Serial2.available())
    Serial2.read();

  uint8_t checksum = command + data;
  uint8_t msg[] = { START_BYTE, command, data, checksum, END_BYTE };
  Logger::log("PicSerial - Sending Message: ", msg, 5);

  Serial2.write(msg, 5);

  // Set timeout to 5 seconds from now.
  timeout_at = millis() + DISPENSER_TIMEOUT_MS;
}

// Send a dispense command to the dispenser.
void PicSerial::sendDispense() {
  sendMessage(SDB_DISPENSE_START, 0x01);
}

// Send a handshake command to the dispenser.
void PicSerial::sendHandshake() {
  sendMessage(SDB_HANDSHAKE, 0x01);
}

// Set the vibration level for the dispenser.
void PicSerial::sendVibrationLevel(uint8_t level) {
  if (level < 0) level = 0;
  if (level > VIBRATION_LEVEL_MAX) level = VIBRATION_LEVEL_MAX;
  sendMessage(SDB_VIBRATE_LEVEL, VIBMODE_U0 + level);
}

// Set the vibration time for the dispenser.
// @param seconds Vibration time in seconds (1-5).
void PicSerial::sendVibrationTime(uint8_t seconds) {
  if (seconds < VIBRATION_DURATION_MIN) seconds = VIBRATION_DURATION_MIN;
  if (seconds > VIBRATION_DURATION_MAX) seconds = VIBRATION_DURATION_MAX;
  sendMessage(SDB_VIBRATE_TIME, VIBDUR_1 + seconds - 1);
}

// Process incoming data from the dispenser.
// @return Command code if valid message received, 0 if no message, -1 if error.
int PicSerial::process() {
  // Check for timeout.
  if (millis() >= timeout_at && timeout_at != 0) {
    timeout_at = 0;  // Reset timeout
    return ACK_ERROR;
  }

  if (Serial2.available() == 0) return 0;

  // Check if first byte is START_BYTE
  if (Serial2.peek() != START_BYTE) {
    Serial2.read();  // Discard invalid byte
    return 0;
  }

  // Check for complete message
  if (Serial2.available() < MSG_LENGTH) return 0;

  uint8_t response[MSG_LENGTH];
  Serial2.readBytes(response, MSG_LENGTH);

  // Validate the response format
  if (response[MSG_SOT] != START_BYTE || response[MSG_EOT] != END_BYTE) {
    return 0;
  }

  timeout_at = 0;
  return response[MSG_COMMAND];
}

// Block until a response is received from the dispenser.
void PicSerial::blockUntilResponse() {
  while (process() == 0)
    delay(DISPENSER_POLL_DELAY_MS);
}
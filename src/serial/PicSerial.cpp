#include "PicSerial.h"
#include "../Utils.h"

// Initialize static members.
unsigned long PicSerial::ack_timeout_at = 0;
unsigned long PicSerial::cycle_complete_timeout_at = 0;
bool PicSerial::waiting_for_fw_data = false;
FirmwareVersionData PicSerial::fw_version_data = {0, 0, 0, false};

// Send a message to the dispenser with command and data bytes.
// @param command Command byte to send.
// @param data Data byte to send.
void PicSerial::sendMessage(byte command, byte data) {
  // Clear serial buffer
  while (Serial2.available())
    Serial2.read();

  uint8_t checksum = command + data;
  uint8_t msg[] = { START_BYTE, command, data, checksum, END_BYTE };

  Serial2.write(msg, 5);

  // Log sent bytes
  Logger::log(F("PIC TX: %02X %02X %02X %02X %02X"), msg[0], msg[1], msg[2], msg[3], msg[4]);

  // Set timeout to 5 seconds from now.
  ack_timeout_at = millis() + DISPENSER_ACK_TIMEOUT_MS;
}

// Send a dispense command to the dispenser.
void PicSerial::sendDispense() {
  fw_version_data.valid = false;  // Invalidate fw version data
  sendMessage(SDB_DISPENSE_START, 0x01);
  // Set cycle completion timeout
  cycle_complete_timeout_at = millis() + DISPENSER_CYCLE_TIMEOUT_MS;
}

// Send a handshake command to the dispenser.
void PicSerial::sendHandshake() {
  fw_version_data.valid = false;  // Invalidate fw version data
  sendMessage(SDB_HANDSHAKE, 0x01);
}

// Set the vibration level for the dispenser.
void PicSerial::sendVibrationLevel(uint8_t level) {
  if (level < 0) level = 0;
  if (level > VIBRATION_LEVEL_MAX) level = VIBRATION_LEVEL_MAX;
  fw_version_data.valid = false;  // Invalidate fw version data
  sendMessage(SDB_VIBRATE_LEVEL, VIBMODE_U0 + level);
}

// Set the vibration time for the dispenser.
// @param seconds Vibration time in seconds (1-5).
void PicSerial::sendVibrationTime(uint8_t seconds) {
  fw_version_data.valid = false;  // Invalidate fw version data
  if (seconds < VIBRATION_DURATION_MIN) seconds = VIBRATION_DURATION_MIN;
  if (seconds > VIBRATION_DURATION_MAX) seconds = VIBRATION_DURATION_MAX;
  sendMessage(SDB_VIBRATE_TIME, VIBDUR_1 + seconds - 1);
}

// Send a query for firmware version information.
void PicSerial::sendQueryFirmwareVersion() {
  sendMessage(SDB_QUERY_STATUS2, 0x00);
  waiting_for_fw_data = false;  // Will be set to true when ACK is received
  fw_version_data.valid = false;  // Invalidate previous data
}

// Get the last received firmware version data.
// @return FirmwareVersionData structure with version information.
FirmwareVersionData PicSerial::getFirmwareVersionData() {
  return fw_version_data;
}

// Process incoming data from the dispenser.
// @return Command code if valid message received, 0 if no message, -1 if error.
int PicSerial::process() {
  // Check for acknowledgment timeout.
  if (millis() >= ack_timeout_at && ack_timeout_at != 0) {
    ack_timeout_at = 0;  // Reset timeout
    return ACK_ERROR;
  }

  // Check for cycle completion timeout.
  if (millis() >= cycle_complete_timeout_at && cycle_complete_timeout_at != 0) {
    cycle_complete_timeout_at = 0;  // Reset timeout
    return CYCLE_TIMEOUT_ERROR;
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

  // Log received bytes
  Logger::log(F("PIC RX: %02X %02X %02X %02X %02X"), response[0], response[1], response[2], response[3], response[4]);

  // Validate the response format
  if (response[MSG_SOT] != START_BYTE || response[MSG_EOT] != END_BYTE) {
    return 0;
  }

  ack_timeout_at = 0;
  
  // Clear cycle timeout if dispense is done
  if (response[MSG_COMMAND] == DISPENSE_DONE) {
    cycle_complete_timeout_at = 0;
  }
  
  // Handle firmware version query ACK and data packet
  if (response[MSG_COMMAND] == SDB_QUERY_STATUS2) {
    // This is the ACK packet, set flag to wait for data packet
    waiting_for_fw_data = true;
    return 0;  // Don't return the command yet, wait for data
  }
  
  // If waiting for firmware data packet, next packet is the data
  if (waiting_for_fw_data) {
    // This is the data packet with firmware version info
    // According to protocol: Byte 1 = Product Type, Byte 2 = Firmware Version, Byte 3 = Cycles Number
    fw_version_data.product_type = response[MSG_COMMAND];   // Byte 1 (product type)
    fw_version_data.firmware_version = response[MSG_DATA1]; // Byte 2 (firmware version)
    fw_version_data.cycles_number = response[MSG_DATA2];    // Byte 3 (cycles number)
    fw_version_data.valid = true;
    waiting_for_fw_data = false;
    return SDB_QUERY_STATUS2;  // Return command to indicate completion
  }
  
  return response[MSG_COMMAND];
}

// Block until a response is received from the dispenser.
void PicSerial::blockUntilResponse() {
  while (process() == 0)
    delay(DISPENSER_POLL_DELAY_MS);
}

// Reset timeouts and clear any incoming messages.
void PicSerial::reset() {
  ack_timeout_at = 0;
  cycle_complete_timeout_at = 0;
  waiting_for_fw_data = false;
  fw_version_data.valid = false;
  
  // Clear serial buffer
  while (Serial2.available())
    Serial2.read();
}
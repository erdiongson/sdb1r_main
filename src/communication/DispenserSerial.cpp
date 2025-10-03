#include "DispenserSerial.h"

// Send a message to the dispenser with command and data bytes.
// @param command Command byte to send.
// @param data Data byte to send.
void DispenserSerial::send_message(byte command, byte data) {
  uint8_t checksum = command + data;
  Serial2.write(START_BYTE);
  Serial2.write(command);
  Serial2.write(data);
  Serial2.write(checksum);
  Serial2.write(END_BYTE);
}

// Send a dispense command to the dispenser.
void DispenserSerial::send_dispense() { send_message(SDB_DISPENSE_START, 0x01); }

// Send a handshake command to the dispenser.
void DispenserSerial::send_handshake() { send_message(SDB_HANDSHAKE, 0x01); }

// Set the vibration level for the dispenser.
void DispenserSerial::send_vibration_level(uint8_t level) {
  byte data;
  switch(level) {
    case 0: data = VIBMODE_U0; break;
    case 1: data = VIBMODE_U1; break;
    case 2: data = VIBMODE_U2; break;
    case 3: data = VIBMODE_U3; break;
    case 4: data = VIBMODE_U4; break;
    default: data = VIBMODE_U2; break; // Default
  }
  
  send_message(SDB_VIBRATE_LEVEL, data);
}

// Set the vibration time for the dispenser.
// @param seconds Vibration time in seconds (1-5).
void DispenserSerial::send_vibration_time(uint8_t seconds) {
  byte data;
  switch(seconds) {
    case 1: data = VIBDUR_1; break;
    case 2: data = VIBDUR_2; break;
    case 3: data = VIBDUR_3; break;
    case 4: data = VIBDUR_4; break;
    case 5: data = VIBDUR_5; break;
    default: data = VIBDUR_2; break; // Default
  }
  
  send_message(SDB_VIBRATE_TIME, data);
}

// Process incoming data from the dispenser.
// @return Command code if valid message received, 0 if no message, -1 if error.
int DispenserSerial::process() {
  if (Serial2.available() >= MSG_LENGTH) {
    uint8_t response[MSG_LENGTH];

    response[MSG_SOT] = Serial2.read();
    response[MSG_COMMAND] = Serial2.read();
    response[MSG_DATA1] = Serial2.read();
    response[MSG_DATA2] = Serial2.read();
    response[MSG_EOT] = Serial2.read();

    String command_name;
    switch(response[MSG_COMMAND]) {
      case SDB_HANDSHAKE: command_name = "SDB_HANDSHAKE"; break;
      case ACKNOWLEDGE: command_name = "ACKNOWLEDGE"; break;
      case VIBRATION_ON: command_name = "VIBRATION_ON"; break;
      case VIBRATION_OFF: command_name = "VIBRATION_OFF"; break;
      case DISPENSE_DONE: command_name = "DISPENSE_DONE"; break;
      case IR_SENSOR_FAILURE: command_name = "IR_SENSOR_FAILURE"; break;
      case MARKER_NOT_DETECTED: command_name = "MARKER_NOT_DETECTED"; break;
      default: command_name = "UNKNOWN(0x" + String(response[MSG_COMMAND], HEX) + ")"; break;
    }
    Serial.println("DispenserSerial::process() - " + command_name + 
                   " <0x" + String(response[MSG_SOT], HEX) + 
                   "><0x" + String(response[MSG_COMMAND], HEX) + 
                   "><0x" + String(response[MSG_DATA1], HEX) + 
                   "><0x" + String(response[MSG_DATA2], HEX) + 
                   "><0x" + String(response[MSG_EOT], HEX) + ">");

    // Validate the response format
    if (response[MSG_SOT] == START_BYTE && response[MSG_EOT] == END_BYTE) {
      // Process the response based on the command
      if (response[MSG_COMMAND] == SDB_HANDSHAKE) {
        return SDB_HANDSHAKE;
      } else if (response[MSG_COMMAND] == ACKNOWLEDGE) {
        // Received acknowledgment
        return ACKNOWLEDGE;
      } else if (response[MSG_COMMAND] == VIBRATION_ON) {
        // Vibration mode turned on
        return VIBRATION_ON;
      } else if (response[MSG_COMMAND] == VIBRATION_OFF) {
        // Vibration mode turned off
        return VIBRATION_OFF;
      } else if (response[MSG_COMMAND] == DISPENSE_DONE) {
        // Dispense completed successfully
        return DISPENSE_DONE;
      } else if (response[MSG_COMMAND] == IR_SENSOR_FAILURE) {
        // IR sensor failure
        return IR_SENSOR_FAILURE;
      } else if (response[MSG_COMMAND] == MARKER_NOT_DETECTED) {
        // Marker not detected
        return MARKER_NOT_DETECTED;
      } else {
        // Unknown message
        return 0;
      }
    }
  }

  // No data available or invalid format
  return 0;
}

// Block until a response is received from the dispenser.
void DispenserSerial::blockUntilResponse() {
  while (process() == 0) delay(10);
}
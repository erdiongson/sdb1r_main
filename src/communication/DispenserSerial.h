#ifndef DISPENSER_SERIAL_H
#define DISPENSER_SERIAL_H

#include "UART.h"

class DispenserSerial {
public:

  DispenserSerial(HardwareSerial& serial)
    : _serial(serial) {}

  bool send_message(byte command, byte data) {
    uint8_t checksum = command + data;
    _serial.write(Serial_SOT);
    _serial.write(command);
    _serial.write(data);
    _serial.write(checksum);
    _serial.write(Serial_EOT);

    return true;
  }

  bool send_dispense() {
    return send_message(SDB_Dispense_START, 0x01);
  }

  bool send_handshake() {
    return send_message(SDB_Handshake, 0x01);
  }

  bool send_vibration_level(uint8_t level) {
    byte data;
    switch(level) {
      case 0: data = VIBMODE_U0; break;
      case 1: data = VIBMODE_U1; break;
      case 2: data = VIBMODE_U2; break;
      case 3: data = VIBMODE_U3; break;
      case 4: data = VIBMODE_U4; break;
      default: data = VIBMODE_U2; break; // Default
    }
    
    return send_message(Vibrate_Mode_ON, data);
  }

  bool send_vibration_time(uint8_t seconds) {
    byte data;
    switch(seconds) {
      case 1: data = VIBDUR_1; break;
      case 2: data = VIBDUR_2; break;
      case 3: data = VIBDUR_3; break;
      case 4: data = VIBDUR_4; break;
      case 5: data = VIBDUR_5; break;
      default: data = VIBDUR_2; break; // Default
    }
    
    return send_message(Set_Vibration_Time, data);
  }

  int process() {
    if (_serial.available() >= BUFFER_SIZE) {
      uint8_t response[MSG_LENGTH];

      response[MSG_SOT] = _serial.read();
      response[MSG_COMMAND] = _serial.read();
      response[MSG_DATA1] = _serial.read();
      response[MSG_DATA2] = _serial.read();
      response[MSG_EOT] = _serial.read();

      String command_name;
      switch(response[MSG_COMMAND]) {
        case SDB_Handshake: command_name = "SDB_Handshake"; break;
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
      if (response[MSG_SOT] == Serial_SOT && response[MSG_EOT] == Serial_EOT) {
        // Process the response based on the command
        if (response[MSG_COMMAND] == SDB_Handshake) {
          return SDB_Handshake;
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
          // Unknown error
          return -1;
        }
      }
    }

    // No data available or invalid format
    return 0;
  }

  bool blockUntilResponse() {
    while (process() == 0) {
      delay(10);
    }
    return true;
  }

private:
  HardwareSerial& _serial;
};

#endif  // DISPENSER_SERIAL_H

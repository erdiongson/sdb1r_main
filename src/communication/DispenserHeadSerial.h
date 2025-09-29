#ifndef DISPENSER_HEAD_SERIAL_H
#define DISPENSER_HEAD_SERIAL_H

#include "UART.h"

class DispenserHeadSerial {
public:

  /**
     * Constructor for DispenserHeadSerial
     * 
     * @param serial Reference to a HardwareSerial for communication
     */
  DispenserHeadSerial(HardwareSerial& serial)
    : _serial(serial) {}

  /**
     * Send a dispense command to the hardware
     * 
     * @param amount Amount to dispense
     * @return true if command was sent, false if busy
     */
  bool send_dispense(int amount) {
    Send cmd;
    cmd.sot = Serial_SOT;
    cmd.command = SDB_Dispense_START;
    cmd.data1 = 0x01;                     // Default value for most commands
    cmd.data2 = cmd.command + cmd.data1;  // Checksum
    cmd.eot = Serial_EOT;

    // Send the command bytes with small delays
    _serial.write(cmd.sot);
    delay(2);
    _serial.write(cmd.command);
    delay(2);
    _serial.write(cmd.data1);
    delay(2);
    _serial.write(cmd.data2);
    delay(2);
    _serial.write(cmd.eot);

    return true;
  }

  bool send_handshake() {
    Send cmd;
    cmd.sot = Serial_SOT;
    cmd.command = SDB_Handshake;
    cmd.data1 = 0x01;                     // Default value for most commands
    cmd.data2 = cmd.command + cmd.data1;  // Checksum
    cmd.eot = Serial_EOT;

    // Send the command bytes with small delays
    _serial.write(cmd.sot);
    delay(2);
    _serial.write(cmd.command);
    delay(2);
    _serial.write(cmd.data1);
    delay(2);
    _serial.write(cmd.data2);
    delay(2);
    _serial.write(cmd.eot);

    return true;
  }

  /**
     * Send a vibration level command to the hardware
     * 
     * @param level Vibration level (0-4)
     * @return true if command was sent
     */
  bool send_vibration_level(uint8_t level) {
    Send cmd;
    cmd.sot = Serial_SOT;
    cmd.command = Vibrate_Mode_ON;
    
    // Set the data1 based on vibration level
    switch(level) {
      case 0:
        cmd.data1 = 0x71;  // VIBMODE_U0
        break;
      case 1:
        cmd.data1 = 0x72;  // VIBMODE_U1
        break;
      case 2:
        cmd.data1 = 0x73;  // VIBMODE_U2
        break;
      case 3:
        cmd.data1 = 0x74;  // VIBMODE_U3
        break;
      case 4:
        cmd.data1 = 0x75;  // VIBMODE_U4
        break;
      default:
        cmd.data1 = 0x73;  // Default to medium level (U2)
        break;
    }
    
    cmd.data2 = cmd.command + cmd.data1;  // Checksum
    cmd.eot = Serial_EOT;

    // Send the command bytes with small delays
    _serial.write(cmd.sot);
    delay(2);
    _serial.write(cmd.command);
    delay(2);
    _serial.write(cmd.data1);
    delay(2);
    _serial.write(cmd.data2);
    delay(2);
    _serial.write(cmd.eot);

    return true;
  }

  /**
     * Send a vibration time command to the hardware
     * 
     * @param seconds Vibration duration in seconds (1-5)
     * @return true if command was sent
     */
  bool send_vibration_time(uint8_t seconds) {
    Send cmd;
    cmd.sot = Serial_SOT;
    cmd.command = Set_Vibration_Time;
    
    // Set the data1 based on vibration time
    switch(seconds) {
      case 1:
        cmd.data1 = 0x81;  // VIBDUR_1
        break;
      case 2:
        cmd.data1 = 0x82;  // VIBDUR_2
        break;
      case 3:
        cmd.data1 = 0x83;  // VIBDUR_3
        break;
      case 4:
        cmd.data1 = 0x84;  // VIBDUR_4
        break;
      case 5:
        cmd.data1 = 0x85;  // VIBDUR_5
        break;
      default:
        cmd.data1 = 0x82;  // Default to 2 seconds
        break;
    }
    
    cmd.data2 = cmd.command + cmd.data1;  // Checksum
    cmd.eot = Serial_EOT;

    // Send the command bytes with small delays
    _serial.write(cmd.sot);
    delay(2);
    _serial.write(cmd.command);
    delay(2);
    _serial.write(cmd.data1);
    delay(2);
    _serial.write(cmd.data2);
    delay(2);
    _serial.write(cmd.eot);

    return true;
  }

  /**
     * Process incoming serial data
     * Should be called regularly to handle responses
     */
  int process() {
    // Check if data is available
    if (_serial.available() >= BUFFER_SIZE) {
      Response resp;

      // Read the response
      resp.sot = _serial.read();
      resp.command = _serial.read();
      resp.data1 = _serial.read();
      resp.data2 = _serial.read();
      resp.eot = _serial.read();

      String command_name;
      switch(resp.command) {
        case SDB_Handshake: command_name = "SDB_Handshake"; break;
        case ACKNOWLEDGE: command_name = "ACKNOWLEDGE"; break;
        case VIBRATION_ON: command_name = "VIBRATION_ON"; break;
        case VIBRATION_OFF: command_name = "VIBRATION_OFF"; break;
        case DISPENSE_DONE: command_name = "DISPENSE_DONE"; break;
        case IR_SENSOR_FAILURE: command_name = "IR_SENSOR_FAILURE"; break;
        case MARKER_NOT_DETECTED: command_name = "MARKER_NOT_DETECTED"; break;
        default: command_name = "UNKNOWN(0x" + String(resp.command, HEX) + ")"; break;
      }
      Serial.println("DispenserHeadSerial::process() - " + command_name + " <0x" + String(resp.sot, HEX) + "><0x" + String(resp.command, HEX) + "><0x" + String(resp.data1, HEX) + "><0x" + String(resp.data2, HEX) + "><0x" + String(resp.eot, HEX) + ">");

      // Validate the response format
      if (resp.sot == Serial_SOT && resp.eot == Serial_EOT) {
        // Process the response based on the command
        if (resp.command == SDB_Handshake) {
          return SDB_Handshake;
        } else if (resp.command == ACKNOWLEDGE) {
          // Received acknowledgment
          return ACKNOWLEDGE;
        } else if (resp.command == VIBRATION_ON) {
          // Vibration mode turned on
          return VIBRATION_ON;
        } else if (resp.command == VIBRATION_OFF) {
          // Vibration mode turned off
          return VIBRATION_OFF;
        } else if (resp.command == DISPENSE_DONE) {
          // Dispense completed successfully
          return DISPENSE_DONE;
        } else if (resp.command == IR_SENSOR_FAILURE) {
          // IR sensor failure
          return IR_SENSOR_FAILURE;
        } else if (resp.command == MARKER_NOT_DETECTED) {
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

#endif  // DISPENSER_HEAD_SERIAL_H

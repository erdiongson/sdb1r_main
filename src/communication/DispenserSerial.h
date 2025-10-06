#ifndef DISPENSER_SERIAL_H
#define DISPENSER_SERIAL_H

#include <Arduino.h>

// Message format position constants
#define MSG_SOT     0  // Start of Transmission position
#define MSG_COMMAND 1  // Command/status byte position
#define MSG_DATA1   2  // Data byte 1 position
#define MSG_DATA2   3  // Data byte 2 (checksum) position
#define MSG_EOT     4  // End of Transmission position
#define MSG_LENGTH  5  // Total message length

#define START_BYTE 0xEF
#define END_BYTE 0xFE

// Command values
#define SDB_HANDSHAKE 0x06       // Handshake command
#define SDB_VIBRATE_LEVEL 0x54     // Command to set vibration mode ON
#define SDB_VIBRATE_TIME 0x65  // Command to set vibration time
#define SDB_DISPENSE_START 0x46  // Command to start dispensing

// Response command values
#define ACKNOWLEDGE 0x46          // Acknowledgment response
#define VIBRATION_ON 0x1C         // Vibration ON response
#define VIBRATION_OFF 0x2C        // Vibration OFF response
#define DISPENSE_DONE 0xF9        // Dispense operation completed
#define IR_SENSOR_FAILURE 0xE1    // Error: IR sensor failure
#define MARKER_NOT_DETECTED 0xE2  // Error: Marker not detected
#define ACK_ERROR 0xE0            // Error: Acknowledgment error

//Vibration Mode Commands
#define VIBMODE_U0 0x71  //former 0xC2, changed for 1RC

//Vibration Duration/Time Commands
#define VIBDUR_1 0x81  //1 second/s

// DispenserSerial class for handling dispenser communication.
class DispenserSerial {
public:
  // Send a dispense command to the dispenser.
  static void send_dispense();
  
  // Send a handshake command to the dispenser.
  static void send_handshake();
  
  // Set the vibration level for the dispenser.
  // @param level The vibration level (0-4).
  static void send_vibration_level(uint8_t level);
  
  // Set the vibration time for the dispenser.
  // @param seconds The vibration time in seconds (1-5).
  static void send_vibration_time(uint8_t seconds);
  
  // Process incoming data from the dispenser.
  // @return Command code if valid message received, 0 if no message, -1 if error.
  static int process();
  
  // Block until a response is received from the dispenser.
  static void blockUntilResponse();

private:
  // Send a message to the dispenser.
  // @param command The command byte.
  // @param data The data byte.
  static void send_message(byte command, byte data);
  
  // Timeout timestamp in milliseconds.
  static unsigned long timeout_at;
};

#endif  // DISPENSER_SERIAL_H

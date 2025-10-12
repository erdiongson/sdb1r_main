#ifndef DISPENSER_SERIAL_H
#define DISPENSER_SERIAL_H

#include <Arduino.h>
#include "../Constants.h"

// Rename constants to match Constants.h
#define START_BYTE DISPENSER_START_BYTE
#define END_BYTE DISPENSER_END_BYTE

// PicSerial class for handling dispenser communication.
class PicSerial {
public:
  // Send a dispense command to the dispenser.
  static void sendDispense();
  
  // Send a handshake command to the dispenser.
  static void sendHandshake();
  
  // Set the vibration level for the dispenser.
  // @param level The vibration level (0-4).
  static void sendVibrationLevel(uint8_t level);
  
  // Set the vibration time for the dispenser.
  // @param seconds The vibration time in seconds (1-5).
  static void sendVibrationTime(uint8_t seconds);
  
  // Process incoming data from the dispenser.
  // @return Command code if valid message received, 0 if no message, -1 if error.
  static int process();
  
  // Block until a response is received from the dispenser.
  static void blockUntilResponse();

private:
  // Send a message to the dispenser.
  // @param command The command byte.
  // @param data The data byte.
  static void sendMessage(byte command, byte data);
  
  // Timeout timestamp in milliseconds.
  static unsigned long timeout_at;
};

#endif  // DISPENSER_SERIAL_H

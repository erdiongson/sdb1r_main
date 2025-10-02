#ifndef DISPENSER_SERIAL_H
#define DISPENSER_SERIAL_H

#include <Arduino.h>
#include "UART.h"

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
};

#endif  // DISPENSER_SERIAL_H

#ifndef DISPENSER_SERIAL_H
#define DISPENSER_SERIAL_H

#include <Arduino.h>
#include "UART.h"

/**
 * @brief DispenserSerial class for handling dispenser communication
 */
class DispenserSerial {
public:
  /**
   * @brief Send a dispense command to the dispenser
   * @return bool - true if the message was sent successfully
   */
  static bool send_dispense();
  
  /**
   * @brief Send a handshake command to the dispenser
   * @return bool - true if the message was sent successfully
   */
  static bool send_handshake();
  
  /**
   * @brief Set the vibration level for the dispenser
   * @param level - vibration level (0-4)
   * @return bool - true if the message was sent successfully
   */
  static bool send_vibration_level(uint8_t level);
  
  /**
   * @brief Set the vibration time for the dispenser
   * @param seconds - vibration time in seconds (1-5)
   * @return bool - true if the message was sent successfully
   */
  static bool send_vibration_time(uint8_t seconds);
  
  /**
   * @brief Process incoming data from the dispenser
   * @return int - command code if valid message received, 0 if no message, -1 if error
   */
  static int process();
  
  /**
   * @brief Block until a response is received from the dispenser
   * @return bool - true when a response is received
   */
  static bool blockUntilResponse();

private:
  /**
   * @brief Send a message to the dispenser
   * @param command - command byte
   * @param data - data byte
   * @return bool - true if the message was sent successfully
   */
  static bool send_message(byte command, byte data);
};

#endif  // DISPENSER_SERIAL_H

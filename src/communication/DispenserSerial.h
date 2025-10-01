#ifndef DISPENSER_SERIAL_H
#define DISPENSER_SERIAL_H

#include <Arduino.h>
#include "UART.h"

class DispenserSerial {
public:
  DispenserSerial(HardwareSerial& serial);
  
  bool send_dispense();
  bool send_handshake();
  bool send_vibration_level(uint8_t level);
  bool send_vibration_time(uint8_t seconds);
  int process();
  bool blockUntilResponse();

private:
  bool send_message(byte command, byte data);
  HardwareSerial& _serial;
};

#endif  // DISPENSER_SERIAL_H

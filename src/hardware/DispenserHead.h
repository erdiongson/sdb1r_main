#ifndef DISPENSER_HEAD_H
#define DISPENSER_HEAD_H

#include "Axis.h"
#include "../communication/DispenserSerial.h"

// Struct for all dispenser head parameters.
struct DispenserHeadParams {
  AxisParams xAxis;
  AxisParams yAxis;
  AxisParams zAxis;

  DispenserHeadParams(
    const AxisParams& xAxis,
    const AxisParams& yAxis,
    const AxisParams& zAxis)
    : xAxis(xAxis),
      yAxis(yAxis),
      zAxis(zAxis) {}
};

class DispenserHead {
public:

  enum DISPENSING_STATE {
    SENT,
    ACKNOWLEDGED,
    COMPLETED
  };

  // Constructor for DispenserHead using DispenserHeadParams.
  // @param params All parameters for the dispenser head.
  DispenserHead(const DispenserHeadParams& params)
    : xAxis(params.xAxis),
      yAxis(params.yAxis),
      zAxis(params.zAxis) {}

  Axis& x() {
    return xAxis;
  }

  Axis& y() {
    return yAxis;
  }

  Axis& z() {
    return zAxis;
  }

  // Send a dispense command to the dispenser.
  void send_dispense() {
    // Don't send a new command if we're still processing the previous one
    if (dispensing_state != COMPLETED) return;

    // Use the serial handler to send the dispense command
    DispenserSerial::send_dispense();
    dispensing_state = SENT;
  }

  // Process incoming data from the dispenser.
  // @return Command code if valid message received, 0 if no message, -1 if error.
  int on_step() {
    int response = DispenserSerial::process();

    // Handle the response based on the returned code
    switch (response) {
      case ACKNOWLEDGE:
        dispensing_state = ACKNOWLEDGED;
        return 0;
      case DISPENSE_DONE:
        dispensing_state = COMPLETED;
        return 0;
      case IR_SENSOR_FAILURE:
        dispensing_state = COMPLETED;
        return IR_SENSOR_FAILURE;
      case MARKER_NOT_DETECTED:
        dispensing_state = COMPLETED;
        return MARKER_NOT_DETECTED;
      case -1:  // Unknown error
        dispensing_state = COMPLETED;
        return -1;
    }
    return 0;
  }

  // Get the current dispensing state
  DISPENSING_STATE get_state() const {
    return dispensing_state;
  }

  // Send a handshake command and wait for response with timeout.
  // @return True if connected, false if timeout occurred.
  bool get_connected() {
    DispenserSerial::send_handshake();

    // Add timeout of 2 seconds (2000ms)
    unsigned long startTime = millis();
    const unsigned long timeout = 2000;  // 2 seconds timeout

    while (DispenserSerial::process() != SDB_Handshake) {
      delay(10);

      // Check if timeout has occurred
      if (millis() - startTime > timeout) {
        return false;  // Timeout occurred
      }
    }

    return true;  // Handshake successful
  }

  // Set vibration level and wait for response.
  // @param level Vibration level (0-4).
  void set_vibration_level(uint8_t level) {
    DispenserSerial::send_vibration_level(level);
    DispenserSerial::blockUntilResponse();
  }

  // Set vibration time and wait for response.
  // @param seconds Vibration duration in seconds (1-5).
  void set_vibration_time(uint8_t seconds) {
    DispenserSerial::send_vibration_time(seconds);
    DispenserSerial::blockUntilResponse();
  }

private:
  Axis xAxis;
  Axis yAxis;
  Axis zAxis;
  DISPENSING_STATE dispensing_state = COMPLETED;
};

#endif  // DISPENSER_HEAD_H

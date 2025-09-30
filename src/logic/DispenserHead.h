#ifndef DISPENSER_HEAD_H
#define DISPENSER_HEAD_H

#include "Axis.h"
#include "../communication/DispenserHeadSerial.h"

/**
 * Struct for all dispenser head parameters
 */
struct DispenserHeadParams {
  AxisParams xAxis;
  AxisParams yAxis;
  AxisParams zAxis;
  HardwareSerial& serial;

  DispenserHeadParams(
    const AxisParams& xAxis,
    const AxisParams& yAxis,
    const AxisParams& zAxis,
    HardwareSerial& serial)
    : xAxis(xAxis),
      yAxis(yAxis),
      zAxis(zAxis),
      serial(serial) {}
};

class DispenserHead {
public:

  enum DISPENSING_STATE {
    SENT,
    ACKNOWLEDGED,
    COMPLETED
  };

  /**
     * Constructor for DispenserHead using DispenserHeadParams
     * 
     * @param params All parameters for the dispenser head
     */
  DispenserHead(const DispenserHeadParams& params)
    : xAxis(params.xAxis),
      yAxis(params.yAxis),
      zAxis(params.zAxis),
      serialHandler(params.serial){};

  Axis& x() {
    return xAxis;
  }

  Axis& y() {
    return yAxis;
  }

  Axis& z() {
    return zAxis;
  }

  void send_dispense() {
    // Don't send a new command if we're still processing the previous one
    if (dispensing_state != COMPLETED) return;

    // Use the serial handler to send the dispense command
    if (serialHandler.send_dispense()) {
      dispensing_state = SENT;
    }
  }

  int on_step() {
    int response = serialHandler.process();

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

  /**
     * Send a handshake command and wait for response with timeout
     * 
     * @return true if connected, false if timeout occurred
     */
  bool get_connected() {
    serialHandler.send_handshake();

    // Add timeout of 2 seconds (2000ms)
    unsigned long startTime = millis();
    const unsigned long timeout = 2000;  // 2 seconds timeout

    while (serialHandler.process() != SDB_Handshake) {
      delay(10);

      // Check if timeout has occurred
      if (millis() - startTime > timeout) {
        return false;  // Timeout occurred
      }
    }

    return true;  // Handshake successful
  }

  /**
   * Set vibration level and wait for response
   * 
   * @param level Vibration level (0-4)
   * @return true if command was sent and response received
   */
  bool set_vibration_level(uint8_t level) {
    if (serialHandler.send_vibration_level(level)) {
      return serialHandler.blockUntilResponse();
    }
    return false;
  }

  /**
   * Set vibration time and wait for response
   * 
   * @param seconds Vibration duration in seconds (1-5)
   * @return true if command was sent and response received
   */
  bool set_vibration_time(uint8_t seconds) {
    if (serialHandler.send_vibration_time(seconds)) {
      return serialHandler.blockUntilResponse();
    }
    return false;
  }

private:
  Axis xAxis;
  Axis yAxis;
  Axis zAxis;
  DispenserHeadSerial serialHandler;
  DISPENSING_STATE dispensing_state = COMPLETED;
};

#endif  // DISPENSER_HEAD_H

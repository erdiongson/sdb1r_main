#ifndef DISPENSER_HEAD_H
#define DISPENSER_HEAD_H

#include "Axis.h"
#include "../../Config.h"
#include "../communication/DispenserSerial.h"

// Struct for dispenser process result containing state and error information.
struct DispenserProcessResult {
  int steppers;
  int dispenser;

  DispenserProcessResult(int steppers, int dispenser)
    : steppers(steppers), dispenser(dispenser) {}
};

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
    if (dispensing_state != DISPENSER_STATE_IDLING) return;

    // Use the serial handler to send the dispense command
    DispenserSerial::send_dispense();
    dispensing_state = DISPENSER_STATE_SENT;
  }

  // Process incoming data from the dispenser.
  // @return DispenserProcessResult containing the updated state and any error code.
  DispenserProcessResult process() {

    #if DEBUG
      return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_IDLING);
    #endif

    // Process stepper motors
    int x_result = this->xAxis.onStep();
    if (x_result == AXIS_STATE_RUNNING) return DispenserProcessResult(AXIS_STATE_RUNNING, DISPENSER_STATE_BLOCKED);
    if (x_result == AXIS_STATE_ERROR_LIMIT_SWITCH) return DispenserProcessResult(AXIS_STATE_ERROR_LIMIT_SWITCH, DISPENSER_STATE_BLOCKED);

    int y_result = this->yAxis.onStep();
    if (y_result == AXIS_STATE_RUNNING) return DispenserProcessResult(AXIS_STATE_RUNNING, DISPENSER_STATE_BLOCKED);
    if (y_result == AXIS_STATE_ERROR_LIMIT_SWITCH) return DispenserProcessResult(AXIS_STATE_ERROR_LIMIT_SWITCH, DISPENSER_STATE_BLOCKED);

    int z_result = this->zAxis.onStep();
    if (z_result == AXIS_STATE_RUNNING) return DispenserProcessResult(AXIS_STATE_RUNNING, DISPENSER_STATE_BLOCKED);
    if (z_result == AXIS_STATE_ERROR_LIMIT_SWITCH) return DispenserProcessResult(AXIS_STATE_ERROR_LIMIT_SWITCH, DISPENSER_STATE_BLOCKED);

    // Process dispenser's serial data
    int response = DispenserSerial::process();

    // Handle the response based on the returned code
    switch (response) {
      case ACKNOWLEDGE:
        dispensing_state = DISPENSER_STATE_ACKNOWLEDGED;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_ACKNOWLEDGED);
      case DISPENSE_DONE:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_IDLING);
      case IR_SENSOR_FAILURE:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE);
      case MARKER_NOT_DETECTED:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED);
    }

    return DispenserProcessResult(AXIS_STATE_COMPLETE, dispensing_state);
  }

  // Get the current dispensing state
  int get_state() const {
    return dispensing_state;
  }

  // Send a handshake command and wait for response with timeout.
  // @return True if connected, false if timeout occurred.
  bool get_connected() {
    DispenserSerial::send_handshake();

    // Add timeout of 2 seconds (2000ms)
    unsigned long startTime = millis();
    const unsigned long timeout = 2000;  // 2 seconds timeout

    while (DispenserSerial::process() != SDB_HANDSHAKE) {
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
  }

  // Set vibration time and wait for response.
  // @param seconds Vibration duration in seconds (1-5).
  void set_vibration_time(uint8_t seconds) {
    DispenserSerial::send_vibration_time(seconds);
  }

private:
  Axis xAxis;
  Axis yAxis;
  Axis zAxis;
  int dispensing_state = DISPENSER_STATE_IDLING;
};

#endif  // DISPENSER_HEAD_H

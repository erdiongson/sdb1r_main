#pragma once

#include "Axis.h"
#include "../../Config.h"
#include "../serial/PicSerial.h"
#include "../Utils.h"

// Struct for dispenser process result containing state and error information.
struct DispenserProcessResult {
  int steppers;
  int dispenser;

  DispenserProcessResult(int steppers, int dispenser) : steppers(steppers), dispenser(dispenser) {}
};

// Struct for all dispenser head parameters.
struct DispenserHeadParams {
  AxisParams x_axis;
  AxisParams y_axis;
  AxisParams z_axis;

  DispenserHeadParams(const AxisParams& x_axis, const AxisParams& y_axis, const AxisParams& z_axis)
      : x_axis(x_axis), y_axis(y_axis), z_axis(z_axis) {}
};

class DispenserHead {
 public:
  // Constructor for DispenserHead using DispenserHeadParams.
  // @param params All parameters for the dispenser head.
  DispenserHead(const DispenserHeadParams& params)
      : x_axis(params.x_axis), y_axis(params.y_axis), z_axis(params.z_axis) {}

  Axis& x() { return x_axis; }
  Axis& y() { return y_axis; }
  Axis& z() { return z_axis; }

  // Send a dispense command to the dispenser.
  void sendDispense() {
    // Don't send a new command if we're still processing the previous one
    if (dispensing_state != DISPENSER_STATE_IDLING) return;

    // Use the serial handler to send the dispense command
    PicSerial::sendDispense();
    dispensing_state = DISPENSER_STATE_SENT;
  }

  // Send a handshake command to the dispenser.
  void sendHandshake() {
    // Don't send a new command if we're still processing the previous one
    if (dispensing_state != DISPENSER_STATE_IDLING) return;

    // Use the serial handler to send the handshake command
    PicSerial::sendHandshake();
    dispensing_state = DISPENSER_STATE_SENT;
  }

  // Set vibration level and wait for response.
  // @param level Vibration level (0-4).
  void setVibrationLevel(uint8_t level) {
    if (dispensing_state != DISPENSER_STATE_IDLING) return;

    PicSerial::sendVibrationLevel(level);
    dispensing_state = DISPENSER_STATE_SENT;
  }

  // Set vibration time and wait for response.
  // @param seconds Vibration duration in seconds (1-5).
  void setVibrationTime(uint8_t seconds) {
    if (dispensing_state != DISPENSER_STATE_IDLING) return;

    PicSerial::sendVibrationTime(seconds);
    dispensing_state = DISPENSER_STATE_SENT;
  }

  // Process incoming data from the dispenser.
  // @return DispenserProcessResult containing the updated state and any error code.
  DispenserProcessResult process() {
#if DEBUG_ONLY_SCREEN
    return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_IDLING);
#endif

    // Process stepper motors
    int z_result = this->z_axis.onStep();
    if (z_result == AXIS_STATE_RUNNING) return DispenserProcessResult(AXIS_STATE_RUNNING, DISPENSER_STATE_BLOCKED);
    if (z_result == AXIS_STATE_ERROR_LIMIT_SWITCH) {
      Logger::log(F("Z axis limit switch triggered unexpectedly"));
      return DispenserProcessResult(AXIS_STATE_ERROR_LIMIT_SWITCH, DISPENSER_STATE_BLOCKED);
    }

    int x_result = this->x_axis.onStep();
    if (x_result == AXIS_STATE_RUNNING) return DispenserProcessResult(AXIS_STATE_RUNNING, DISPENSER_STATE_BLOCKED);
    if (x_result == AXIS_STATE_ERROR_LIMIT_SWITCH) {
      Logger::log(F("X axis limit switch triggered unexpectedly"));
      return DispenserProcessResult(AXIS_STATE_ERROR_LIMIT_SWITCH, DISPENSER_STATE_BLOCKED);
    }

    int y_result = this->y_axis.onStep();
    if (y_result == AXIS_STATE_RUNNING) return DispenserProcessResult(AXIS_STATE_RUNNING, DISPENSER_STATE_BLOCKED);
    if (y_result == AXIS_STATE_ERROR_LIMIT_SWITCH) {
      Logger::log(F("Y axis limit switch triggered unexpectedly"));
      return DispenserProcessResult(AXIS_STATE_ERROR_LIMIT_SWITCH, DISPENSER_STATE_BLOCKED);
    }

    // If not expecting any dispenser response, skip processing
    if (dispensing_state == DISPENSER_STATE_IDLING)
      return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_IDLING);

    // Process dispenser's serial data
    int response = PicSerial::process();

    // Handle the response based on the returned code
    switch (response) {
      case SDB_HANDSHAKE:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_IDLING);
      case SDB_VIBRATE_LEVEL:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_IDLING);
      case SDB_VIBRATE_TIME:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_IDLING);
      case ACKNOWLEDGE:
        dispensing_state = DISPENSER_STATE_ACKNOWLEDGED;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_ACKNOWLEDGED);
      case DISPENSE_DONE:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_IDLING);
      case ACK_ERROR:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_ERROR_ACK_ERROR);
      case IR_SENSOR_FAILURE:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE);
      case MARKER_NOT_DETECTED:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED);
      case CYCLE_TIMEOUT_ERROR:
        dispensing_state = DISPENSER_STATE_IDLING;
        return DispenserProcessResult(AXIS_STATE_COMPLETE, DISPENSER_STATE_ERROR_CYCLES_TIMEOUT);
    }

    return DispenserProcessResult(AXIS_STATE_COMPLETE, dispensing_state);
  }

  // Get the current dispensing state
  int getState() const { return dispensing_state; }

  // Clear axes if at limit positions by moving them away.
  void clearLimits() {
    if (x_axis.isAtMin()) x_axis.moveBy(STEPS_PER_UNIT_X * 10);
    if (y_axis.isAtMin()) y_axis.moveBy(STEPS_PER_UNIT_Y * 10);
    bool zAtMax = z_axis.isAtMax();
    Logger::log("Z is at max already?" + String(zAtMax));
    if (zAtMax) {
      Logger::log("Clearing Z");
      z_axis.moveBy(-STEPS_PER_UNIT_Z * 10);
    }
  }

 private:
  Axis x_axis;
  Axis y_axis;
  Axis z_axis;
  int dispensing_state = DISPENSER_STATE_IDLING;
};

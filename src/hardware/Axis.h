#ifndef AXIS_H
#define AXIS_H

#include "Arduino.h"
#include <AccelStepper.h>

#define AXIS_STATE_COMPLETE 0
#define AXIS_STATE_RUNNING 1
#define AXIS_STATE_ERROR_LIMIT_SWITCH 2

/**
 * Struct for complete axis configuration parameters
 */
struct AxisParams {
  int stepPin;
  int dirPin;
  int minPin;
  int maxPin;
  float maxSpeed;
  float acceleration;

  AxisParams(
    int stepPin,
    int dirPin,
    int minPin,
    int maxPin,
    float maxSpeed = 1000.0,
    float acceleration = 500.0)
    : stepPin(stepPin),
      dirPin(dirPin),
      minPin(minPin),
      maxPin(maxPin),
      maxSpeed(maxSpeed),
      acceleration(acceleration) {}
};

class Axis {
private:
  AccelStepper stepper;
  int minLimitPin;
  int maxLimitPin;
  bool movingPositive;  // true if moving in positive direction, false if negative
  bool enabled;         // true if axis is enabled, false if disabled

public:
  /**
     * Constructor for Axis class using AxisParams struct
     * 
     * @param params Struct containing all axis configuration parameters
     */
  Axis(const AxisParams& params)
    : stepper(1, params.stepPin, params.dirPin),  // 1 = DRIVER interface (step/dir)
      minLimitPin(params.minPin),
      maxLimitPin(params.maxPin),
      movingPositive(false),
      enabled(true) {

    // Configure limit switch pins as inputs with pull-up resistors
    pinMode(minLimitPin, INPUT_PULLUP);
    pinMode(maxLimitPin, INPUT_PULLUP);

    // Configure stepper motor parameters
    stepper.setMaxSpeed(params.maxSpeed);
    stepper.setAcceleration(params.acceleration);
  }


  void reset() {
    stepper.setCurrentPosition(0);
  }

  void setDisabled(bool disabled) {
    enabled = !disabled;
  }

  void moveToMax() {
    stepper.move(999999999);
    movingPositive = true;
  }

  void moveToMin() {
    stepper.move(-999999999);
    movingPositive = false;
  }

  void moveBy(long position) {
    stepper.move(position);
    movingPositive = (position > 0);
  }

  void moveTo(long position) {
    stepper.moveTo(position);
    movingPositive = (position > stepper.currentPosition());
  }

  void stop() {
    stepper.stop();
  }

  bool isAtMin() {
    return digitalRead(minLimitPin) == LOW;
  }

  bool isAtMax() {
    return digitalRead(maxLimitPin) == LOW;
  }

  // Process one step of the motor movement.
  // This function should be called frequently in the main loop.
  // @return AxisStepResult containing the completion state and any error code.
  int onStep() {

    // Check limit switches and prevent movement in that direction if triggered
    if (isComplete())
      return AXIS_STATE_COMPLETE;

    stepper.run();
    return AXIS_STATE_RUNNING;
  }

  AccelStepper& getStepper() {
    return stepper;
  }

  bool isComplete() {
    // Always complete if disabled
    if (!enabled)
      return true;

    bool result = ((movingPositive && digitalRead(maxLimitPin) == LOW) || (!movingPositive && digitalRead(minLimitPin) == LOW) || stepper.distanceToGo() == 0);
    if (result) { stop(); }
    return result;
  }

  void runUntilCompleteBlocking() {
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
  }

  long getCurrentPosition() {
    return stepper.currentPosition();
  }
};

#endif  // AXIS_H
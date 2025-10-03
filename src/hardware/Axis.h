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
  bool running = false;
  bool to_limit = false;
  bool prevMinState = false;  // Previous state of min limit switch (true = hit)
  bool prevMaxState = false;  // Previous state of max limit switch (true = hit)

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
    if (!enabled) return;
    if (isAtMax()) return;  // Already at max limit
    running = true;
    to_limit = true;
    stepper.move(999999999);
    movingPositive = true;

    prevMinState = isAtMin();
    prevMaxState = isAtMax();
  }

  void moveToMin() {
    if (!enabled) return;
    if (isAtMin()) return;  // Already at min limit
    running = true;
    to_limit = true;
    stepper.move(-999999999);
    movingPositive = false;

    prevMinState = isAtMin();
    prevMaxState = isAtMax();
  }

  void moveBy(long position) {
    if (!enabled) return;
    if (position > 0 && isAtMax()) return;  // Trying to move positive but at max limit
    if (position < 0 && isAtMin()) return;  // Trying to move negative but at min limit
    running = true;
    to_limit = false;
    stepper.move(position);
    movingPositive = (position > 0);

    prevMinState = isAtMin();
    prevMaxState = isAtMax();
  }

  void moveTo(long position) {
    if (!enabled) return;
    bool wouldMovePositive = (position > stepper.currentPosition());
    if (wouldMovePositive && isAtMax()) return;  // Trying to move positive but at max limit
    if (!wouldMovePositive && isAtMin()) return;  // Trying to move negative but at min limit
    running = true;
    to_limit = false;
    stepper.moveTo(position);
    movingPositive = wouldMovePositive;

    prevMinState = isAtMin();
    prevMaxState = isAtMax();
  }

  void stop() {
    if (!enabled) return;
    stepper.stop();
  }

  bool isAtMin() {
    return digitalRead(minLimitPin) == LOW;
  }

  bool isAtMax() {
    return digitalRead(maxLimitPin) == LOW;
  }

  // Returns true if the min limit switch transitioned from not-hit to hit.
  // Updates the stored previous state.
  // @return True if switch just got hit, false otherwise.
  bool didHitMin() {
    bool currentState = isAtMin();
    bool justHit = !prevMinState && currentState;
    prevMinState = currentState;
    return justHit;
  }

  // Returns true if the max limit switch transitioned from not-hit to hit.
  // Updates the stored previous state.
  // @return True if switch just got hit, false otherwise.
  bool didHitMax() {
    bool currentState = isAtMax();
    bool justHit = !prevMaxState && currentState;
    prevMaxState = currentState;
    return justHit;
  }

  // Process one step of the motor movement.
  // This function should be called frequently in the main loop.
  // @return AxisStepResult containing the completion state and any error code.
  int onStep() {
    if (!running) return AXIS_STATE_COMPLETE;

    // Check limit switches and prevent movement in that direction if triggered
    if (didHitMin()) {
      stopRunning();
      if (to_limit && !movingPositive) {
        // Expected, should stop
        return AXIS_STATE_COMPLETE;
      } else {
        // Not expected, error
        Serial.println("Hit min!!!");
        return AXIS_STATE_ERROR_LIMIT_SWITCH;
      }
    }
    if (didHitMax()) {
      stopRunning();
      if (to_limit && movingPositive) {
        // Expected, should stop
        return AXIS_STATE_COMPLETE;
      } else {
        // Not expected, error
        Serial.println("Hit max!!!");
        return AXIS_STATE_ERROR_LIMIT_SWITCH;
      }
    }

    bool is_running = stepper.run();

    if (!is_running) {
      // Expected, should stop
      stopRunning();
      return AXIS_STATE_COMPLETE;
    }

    return AXIS_STATE_RUNNING;
  }

  void stopRunning() {
    if (!enabled) return;
    running = false;
    stepper.move(0); // Sets the target position to the current position;
    stepper.setAcceleration(0);
    stepper.setSpeed(0);
    stepper.runToPosition();
  }

  AccelStepper& getStepper() {
    return stepper;
  }

  void runUntilCompleteBlocking() {
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
  }
};

#endif  // AXIS_H
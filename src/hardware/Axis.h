#pragma once

#include "Arduino.h"
#include <AccelStepper.h>
#include "../Constants.h"
#include "../Utils.h"

// Struct for complete axis configuration parameters.
struct AxisParams {
  int step_pin;
  int dir_pin;
  int min_pin;
  int max_pin;
  float max_speed;
  float acceleration;

  AxisParams(int step_pin, int dir_pin, int min_pin, int max_pin, float max_speed = 1000.0, float acceleration = 500.0)
      : step_pin(step_pin),
        dir_pin(dir_pin),
        min_pin(min_pin),
        max_pin(max_pin),
        max_speed(max_speed),
        acceleration(acceleration) {}
};

class Axis {
 private:
  AccelStepper stepper;
  int min_limit_pin;
  int max_limit_pin;
  bool moving_positive;  // true if moving in positive direction, false if negative
  bool enabled;          // true if axis is enabled, false if disabled
  bool running = false;
  bool to_limit = false;
  bool prev_min_state = false;  // Previous state of min limit switch (true = hit)
  bool prev_max_state = false;  // Previous state of max limit switch (true = hit)
  int min_counter = 0;          // Debounce counter for min limit switch
  int max_counter = 0;          // Debounce counter for max limit switch

 public:
  // Constructor for Axis class using AxisParams struct.
  // @param params Struct containing all axis configuration parameters.
  Axis(const AxisParams& params)
      : stepper(1, params.step_pin, params.dir_pin),  // 1 = DRIVER interface (step/dir)
        min_limit_pin(params.min_pin),
        max_limit_pin(params.max_pin),
        moving_positive(false),
        enabled(true) {
    // Configure limit switch pins as inputs with pull-up resistors
    pinMode(min_limit_pin, INPUT_PULLUP);
    pinMode(max_limit_pin, INPUT_PULLUP);

    // Configure stepper motor parameters
    stepper.setMaxSpeed(params.max_speed);
    stepper.setAcceleration(params.acceleration);
  }

  void reset() { stepper.setCurrentPosition(0); }

  void setDisabled(bool disabled) { enabled = !disabled; }

  void moveToMax() {
    if (!enabled) return;
    if (isAtMax()) return;  // Already at max limit
    running = true;
    to_limit = true;
    stepper.move(999999999);
    moving_positive = true;

    prev_min_state = isAtMin();
    prev_max_state = isAtMax();
  }

  void moveToMin() {
    if (!enabled) return;
    if (isAtMin()) return;  // Already at min limit
    running = true;
    to_limit = true;
    stepper.move(-999999999);
    moving_positive = false;

    prev_min_state = isAtMin();
    prev_max_state = isAtMax();
  }

  void moveBy(long position) {
    if (!enabled) return;
    if (position > 0 && isAtMax()) return;  // Trying to move positive but at max limit
    if (position < 0 && isAtMin()) return;  // Trying to move negative but at min limit
    running = true;
    to_limit = false;
    stepper.move(position);
    moving_positive = (position > 0);

    prev_min_state = isAtMin();
    prev_max_state = isAtMax();

    Logger::log("Moving to " + String(position));
    Logger::log("Is at max?" + String(prev_max_state));
  }

  void moveTo(long position) {
    if (!enabled) return;
    bool wouldMovePositive = (position > stepper.currentPosition());
    if (wouldMovePositive && isAtMax()) return;   // Trying to move positive but at max limit
    if (!wouldMovePositive && isAtMin()) return;  // Trying to move negative but at min limit
    running = true;
    to_limit = false;
    stepper.moveTo(position);
    moving_positive = wouldMovePositive;

    prev_min_state = isAtMin();
    prev_max_state = isAtMax();
  }

  void stop() {
    if (!enabled) return;
    stepper.stop();
  }

  bool isAtMin() { return digitalRead(min_limit_pin) == LOW; }

  bool isAtMax() { return digitalRead(max_limit_pin) == LOW; }

  // Returns true if the min limit switch transitioned from not-hit to hit.
  // Uses debouncing logic requiring 3 steady states before transitioning.
  // Updates the stored previous state.
  // @return True if switch just got hit, false otherwise.
  bool didHitMin() {
    bool rawState = isAtMin();
    
    // Update counter based on raw state
    if (rawState) {
      if (min_counter < 3) min_counter++;
    } else {
      if (min_counter > -3) min_counter--;
    }
    
    // Determine debounced current state
    bool currentState = prev_min_state;
    if (min_counter >= 3) {
      currentState = true;
    } else if (min_counter <= -3) {
      currentState = false;
    }
    
    // Detect transition from not-hit to hit
    bool justHit = !prev_min_state && currentState;
    prev_min_state = currentState;
    return justHit;
  }

  // Returns true if the max limit switch transitioned from not-hit to hit.
  // Uses debouncing logic requiring 3 steady states before transitioning.
  // Updates the stored previous state.
  // @return True if switch just got hit, false otherwise.
  bool didHitMax() {
    bool rawState = isAtMax();
    
    // Update counter based on raw state
    if (rawState) {
      if (max_counter < 3) max_counter++;
    } else {
      if (max_counter > -3) max_counter--;
    }
    
    // Determine debounced current state
    bool currentState = prev_max_state;
    if (max_counter >= 3) {
      currentState = true;
    } else if (max_counter <= -3) {
      currentState = false;
    }
    
    // Detect transition from not-hit to hit
    bool justHit = !prev_max_state && currentState;
    prev_max_state = currentState;
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
      if (to_limit && !moving_positive) {
        // Expected, should stop
        return AXIS_STATE_COMPLETE;
      } else {
        // Not expected, error
        Logger::log(F("Min limit switch triggered unexpectedly"));
        return AXIS_STATE_ERROR_LIMIT_SWITCH;
      }
    }
    if (didHitMax()) {
      Logger::log("Max limit switch triggered");
      stopRunning();
      if (to_limit && moving_positive) {
        // Expected, should stop
        return AXIS_STATE_COMPLETE;
      } else {
        // Not expected, error
        Logger::log(F("Max limit switch triggered unexpectedly"));
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
    stepper.move(0);  // Sets the target position to the current position;
    stepper.setAcceleration(0);
    stepper.setSpeed(0);
    stepper.runToPosition();
  }

  AccelStepper& getStepper() { return stepper; }

  void runUntilCompleteBlocking() {
    while (onStep() == AXIS_STATE_RUNNING) {}
  }
};
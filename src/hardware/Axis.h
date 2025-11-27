#pragma once

#include "Arduino.h"
#include <AccelStepper.h>
#include "../Constants.h"
#include "../Utils.h"
#include "../../Config.h"

// Struct for complete axis configuration parameters.
struct AxisParams {
  int step_pin;
  int dir_pin;
  int min_pin;
  int max_pin;
  volatile uint8_t* min_port;  // Port register for min limit (e.g., &PINL)
  uint8_t min_bit;              // Bit position in port (0-7)
  volatile uint8_t* max_port;  // Port register for max limit (e.g., &PINA)
  uint8_t max_bit;              // Bit position in port (0-7)
  float max_speed;
  float acceleration;
  long steps_per_unit;

  AxisParams(int step_pin, int dir_pin, int min_pin, int max_pin, 
             volatile uint8_t* min_port, uint8_t min_bit,
             volatile uint8_t* max_port, uint8_t max_bit,
             float max_speed, float acceleration, long steps_per_unit)
      : step_pin(step_pin),
        dir_pin(dir_pin),
        min_pin(min_pin),
        max_pin(max_pin),
        min_port(min_port),
        min_bit(min_bit),
        max_port(max_port),
        max_bit(max_bit),
        max_speed(max_speed),
        acceleration(acceleration),
        steps_per_unit(steps_per_unit) {}
};

class Axis {
 private:
  AccelStepper stepper;
  int min_limit_pin;
  int max_limit_pin;
  volatile uint8_t* min_limit_port;  // Port register for direct access
  uint8_t min_limit_bit;             // Bit position in port
  volatile uint8_t* max_limit_port;  // Port register for direct access
  uint8_t max_limit_bit;             // Bit position in port
  bool moving_positive;  // true if moving in positive direction, false if negative
  bool enabled;          // true if axis is enabled, false if disabled
  bool running = false;
  bool to_limit = false;
  bool prev_min_state = false;  // Previous state of min limit switch (true = hit)
  bool prev_max_state = false;  // Previous state of max limit switch (true = hit)
  int min_counter = 0;          // Debounce counter for min limit switch
  int max_counter = 0;          // Debounce counter for max limit switch
  float max_speed;              // Maximum speed for the axis
  float max_acceleration;       // Maximum acceleration for the axis
  long steps_per_unit;

 public:
  // Constructor for Axis class using AxisParams struct.
  // @param params Struct containing all axis configuration parameters.
  Axis(const AxisParams& params)
      : stepper(1, params.step_pin, params.dir_pin),  // 1 = DRIVER interface (step/dir)
        min_limit_pin(params.min_pin),
        max_limit_pin(params.max_pin),
        min_limit_port(params.min_port),
        min_limit_bit(params.min_bit),
        max_limit_port(params.max_port),
        max_limit_bit(params.max_bit),
        moving_positive(false),
        enabled(true),
        max_speed(params.max_speed),
        max_acceleration(params.acceleration),
        steps_per_unit(params.steps_per_unit) {
          
    // Configure limit switch pins as inputs with pull-up resistors
    pinMode(min_limit_pin, INPUT_PULLUP);
    pinMode(max_limit_pin, INPUT_PULLUP);

    // Configure stepper motor parameters
    stepper.setMaxSpeed(max_speed);
    stepper.setAcceleration(max_acceleration);
  }

  void reset() {
    Logger::log(F("Setting current position to 0")); 
    stepper.setCurrentPosition(0);
  }

  void setDisabled(bool disabled) { enabled = !disabled; }

  int moveToMax() {
    if (!enabled) return AXIS_STATE_COMPLETE;
    if (isAtMax()) return AXIS_STATE_ERROR_LIMIT_SWITCH;  // Already at max limit
    running = true;
    to_limit = true;
    stepper.move(999999999);
    moving_positive = true;

    prev_min_state = isAtMin();
    prev_max_state = isAtMax();
    return AXIS_STATE_RUNNING;
  }

  int moveToMin() {
    if (!enabled) return AXIS_STATE_COMPLETE;
    if (isAtMin()) return AXIS_STATE_ERROR_LIMIT_SWITCH;  // Already at min limit
    running = true;
    to_limit = true;
    stepper.move(-999999999);
    moving_positive = false;

    prev_min_state = isAtMin();
    prev_max_state = isAtMax();
    return AXIS_STATE_RUNNING;
  }

int moveBy(long position) {
    if (!enabled) return AXIS_STATE_COMPLETE;
    if (position == 0) return AXIS_STATE_COMPLETE;
    if (position > 0 && isAtMax()) {
      Logger::log(F("Trying to move positive but at max limit (moveBy)"));
      return AXIS_STATE_ERROR_LIMIT_SWITCH;  // Trying to move positive but at max limit
    }
    if (position < 0 && isAtMin()) {
      Logger::log(F("Trying to move negative but at min limit (moveBy)"));
      return AXIS_STATE_ERROR_LIMIT_SWITCH;  // Trying to move negative but at min limit
    }
    running = true;
    to_limit = false;
    stepper.move(position);
    moving_positive = (position > 0);

    prev_min_state = isAtMin();
    prev_max_state = isAtMax();
    return AXIS_STATE_RUNNING;
  }

int moveTo(long position) {
    if (!enabled) return AXIS_STATE_COMPLETE;

    long current_position = stepper.currentPosition();
    Logger::log(F("Current position: %ld, Target position: %ld"), current_position, position);

    if (position == current_position) {
      Logger::log(F("Already at target position"));
      return AXIS_STATE_COMPLETE;
    }

    bool would_move_positive = (position > current_position);
    if (would_move_positive && isAtMax()) {
      Logger::log(F("Trying to move positive but at max limit (moveTo)"));
      return AXIS_STATE_ERROR_LIMIT_SWITCH;   // Trying to move positive but at max limit
    }
    if (!would_move_positive && isAtMin()) {
      Logger::log(F("Trying to move negative but at min limit (moveTo)"));
      return AXIS_STATE_ERROR_LIMIT_SWITCH;  // Trying to move negative but at min limit
    }
    running = true;
    to_limit = false;
    stepper.moveTo(position);
    moving_positive = would_move_positive;

    prev_min_state = isAtMin();
    prev_max_state = isAtMax();
    return AXIS_STATE_RUNNING;
  }

  void stop() {
    if (!enabled) return;
    stepper.stop();
  }

  // Checks if min limit switch is triggered using direct port access.
  // @return True if limit switch is active (LOW), false otherwise.
  bool isAtMin() { return !(*min_limit_port & _BV(min_limit_bit)); }

  // Checks if max limit switch is triggered using direct port access.
  // @return True if limit switch is active (LOW), false otherwise.
  bool isAtMax() { return !(*max_limit_port & _BV(max_limit_bit)); }

  // Returns true if the min limit switch transitioned from not-hit to hit.
  // Uses debouncing logic requiring 3 steady states before transitioning.
  // Updates the stored previous state.
  // @return True if switch just got hit, false otherwise.
  bool didHitMin() {
    bool raw_state = isAtMin();
    
    // Update counter based on raw state
    if (raw_state) {
      if (min_counter < 10) min_counter++;
    } else {
      if (min_counter > -10) min_counter--;
    }
    
    // Determine debounced current state
    bool current_state = prev_min_state;
    if (min_counter >= 10) {
      current_state = true;
    } else if (min_counter <= -10) {
      current_state = false;
    }
    
    // Detect transition from not-hit to hit
    bool just_hit = !prev_min_state && current_state;
    prev_min_state = current_state;
    return just_hit;
  }

  // Returns true if the max limit switch transitioned from not-hit to hit.
  // Uses debouncing logic requiring 3 steady states before transitioning.
  // Updates the stored previous state.
  // @return True if switch just got hit, false otherwise.
  bool didHitMax() {
    bool raw_state = isAtMax();
    
    // Update counter based on raw state
    if (raw_state) {
      if (max_counter < 10) max_counter++;
    } else {
      if (max_counter > -10) max_counter--;
    }
    
    // Determine debounced current state
    bool current_state = prev_max_state;
    if (max_counter >= 10) {
      current_state = true;
    } else if (max_counter <= -10) {
      current_state = false;
    }
    
    // Detect transition from not-hit to hit
    bool just_hit = !prev_max_state && current_state;
    prev_max_state = current_state;
    return just_hit;
  }

  // Process one step of the motor movement.
  // This function should be called frequently in the main loop.
  // @return AxisStepResult containing the completion state and any error code.
  int onStep() {
    if (!running) return AXIS_STATE_COMPLETE;

    // Check limit switches and prevent movement in that direction if triggered
    if (didHitMin()) {
      // Not even the correct direction
      if (moving_positive) {
        stopRunning();
        Logger::log(F("Min limit switch triggered unexpectedly (new position: %ld)"), stepper.currentPosition());
        return AXIS_STATE_ERROR_LIMIT_SWITCH;
      }

      // Typically used for homing / zeroing, indicating the axis has reached the limit switch
      if (to_limit) {
        stopRunning();
        Logger::log(F("Min limit switch triggered as expected (new position: %ld)"), stepper.currentPosition());
        return AXIS_STATE_COMPLETE;
      }
     
      // Edge case: If limit switch is hit but it is close to the target position, continue
      if (stepper.distanceToGo() < (steps_per_unit * LIMIT_SWITCH_THRESHOLD_MM)) {
        return AXIS_STATE_RUNNING;
      }

      // No other legitimate reason to hit the limit switch
      return AXIS_STATE_ERROR_LIMIT_SWITCH;
    }

    if (didHitMax()) {
      // Not even the correct direction
      if (!moving_positive) {
        stopRunning();
        Logger::log(F("Max limit switch triggered unexpectedly (new position: %ld)"), stepper.currentPosition());
        return AXIS_STATE_ERROR_LIMIT_SWITCH;
      }

      // Typically used for homing / zeroing, indicating the axis has reached the limit switch
      if (to_limit) {
        stopRunning();
        Logger::log(F("Max limit switch triggered as expected (new position: %ld)"), stepper.currentPosition());
        return AXIS_STATE_COMPLETE;
      }
     
      // Edge case: If limit switch is hit but it is close to the target position, continue
      if (stepper.distanceToGo() < (steps_per_unit * LIMIT_SWITCH_THRESHOLD_MM)) {
        return AXIS_STATE_RUNNING;
      }

      // No other legitimate reason to hit the limit switch
      return AXIS_STATE_ERROR_LIMIT_SWITCH;
    }

    bool is_running = stepper.run();

    // Speed is 0, and distance to target is 0
    if (!is_running) {
      running = false;

      return AXIS_STATE_COMPLETE;
    }

    return AXIS_STATE_RUNNING;
  }

  void stopRunning() {
    if (!enabled) return;
    running = false;

    // Signal stepper to stop, decelerating until speed = 0
    stepper.stop();

    // Set current position as target
    stepper.setCurrentPosition(stepper.currentPosition());
  }

  AccelStepper& getStepper() { return stepper; }

  // Runs the axis until movement is complete, blocking execution.
  // @return The number of stepper.run() calls made during the movement.
  unsigned long runUntilCompleteBlocking() {
    unsigned long step_count = 0;
    while (onStep() == AXIS_STATE_RUNNING) {
      step_count++;
    }
    return step_count;
  }

  // Gets the maximum speed for the axis.
  // @return The maximum speed value.
  float getMaxSpeed() const { return max_speed; }

  // Gets the maximum acceleration for the axis.
  // @return The maximum acceleration value.
  float getMaxAcceleration() const { return max_acceleration; }

  // Sets the maximum speed for the axis.
  // @param speed The new maximum speed value.
  void setMaxSpeed(float speed) {
    max_speed = speed;
    stepper.setMaxSpeed(max_speed);
  }

  // Sets the maximum acceleration for the axis.
  // @param accel The new maximum acceleration value.
  void setMaxAcceleration(float accel) {
    max_acceleration = accel;
    stepper.setAcceleration(max_acceleration);
  }
};
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
  volatile uint8_t* min_port;  // Port register for min limit (e.g., &PINL)
  uint8_t min_bit;              // Bit position in port (0-7)
  volatile uint8_t* max_port;  // Port register for max limit (e.g., &PINA)
  uint8_t max_bit;              // Bit position in port (0-7)
  float max_speed;
  float acceleration;

  AxisParams(int step_pin, int dir_pin, int min_pin, int max_pin, 
             volatile uint8_t* min_port, uint8_t min_bit,
             volatile uint8_t* max_port, uint8_t max_bit,
             float max_speed = 1000.0, float acceleration = 500.0)
      : step_pin(step_pin),
        dir_pin(dir_pin),
        min_pin(min_pin),
        max_pin(max_pin),
        min_port(min_port),
        min_bit(min_bit),
        max_port(max_port),
        max_bit(max_bit),
        max_speed(max_speed),
        acceleration(acceleration) {}
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
        max_acceleration(params.acceleration) {
    // Configure limit switch pins as inputs with pull-up resistors
    pinMode(min_limit_pin, INPUT_PULLUP);
    pinMode(max_limit_pin, INPUT_PULLUP);

    // Configure stepper motor parameters
    stepper.setMaxSpeed(max_speed);
    stepper.setAcceleration(max_acceleration);
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
  }

  void moveTo(long position) {
    if (!enabled) return;
    bool would_move_positive = (position > stepper.currentPosition());
    if (would_move_positive && isAtMax()) return;   // Trying to move positive but at max limit
    if (!would_move_positive && isAtMin()) return;  // Trying to move negative but at min limit
    running = true;
    to_limit = false;
    stepper.moveTo(position);
    moving_positive = would_move_positive;

    prev_min_state = isAtMin();
    prev_max_state = isAtMax();
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
      stopRunning();
      if (to_limit && !moving_positive) {
        // Expected, should stop
        return AXIS_STATE_COMPLETE;
      } else {
        // Not expected, error
        return AXIS_STATE_ERROR_LIMIT_SWITCH;
      }
    }
    if (didHitMax()) {
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
    // Force immediate stop by setting target to current position
    long current_pos = stepper.currentPosition();
    stepper.setCurrentPosition(current_pos);  // Resets both current and target to same value
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
#pragma once

#include <avr/pgmspace.h>

// =============================================================================
// SECURITY SETTINGS
// =============================================================================
const char SUPER_PASSWORD[] PROGMEM = "superXQ";
const char INITIAL_PASSWORD[] PROGMEM = "init1234";

// =============================================================================
// UI TIMING SETTINGS
// =============================================================================
const unsigned long DIALOG_DISPLAY_DURATION_MS = 2000;
const unsigned long ERROR_DISPLAY_DURATION_MS = 3000;
const unsigned long KEYBOARD_TRANSITION_DELAY_MS = 200;
const unsigned long KEYBOARD_ERROR_DISPLAY_MS = 1000;

// =============================================================================
// DISPENSER SETTINGS
// =============================================================================
// Defines the number of "dispenses" performed during priming.
const int PRIME_DISPENSE_NUM = 2;

// Serial communication timeouts (in milliseconds)
const unsigned long DISPENSER_ACK_TIMEOUT_MS = 5000;
const unsigned long DISPENSER_CYCLE_TIMEOUT_MS = 30000;
const unsigned long DISPENSER_POLL_DELAY_MS = 10;

// =============================================================================
// AXIS LIMIT SWITCH PINS
// =============================================================================
const int LIMIT_S_X_MIN = 46;
const int LIMIT_S_X_MAX = 22;

const int LIMIT_S_Y_MIN = 32;
const int LIMIT_S_Y_MAX = 44;

const int LIMIT_S_Z_MIN = 38;
const int LIMIT_S_Z_MAX = 36;

// Port/bit mappings for direct port access (Arduino Mega 2560)
// Pin 22 -> PA0, Pin 32 -> PC5, Pin 36 -> PC1
// Pin 38 -> PD7, Pin 44 -> PL5, Pin 46 -> PL3
#define LIMIT_S_X_MIN_PORT PINL
#define LIMIT_S_X_MIN_BIT 3

#define LIMIT_S_X_MAX_PORT PINA
#define LIMIT_S_X_MAX_BIT 0

#define LIMIT_S_Y_MIN_PORT PINC
#define LIMIT_S_Y_MIN_BIT 5

#define LIMIT_S_Y_MAX_PORT PINL
#define LIMIT_S_Y_MAX_BIT 5

#define LIMIT_S_Z_MIN_PORT PIND
#define LIMIT_S_Z_MIN_BIT 7

#define LIMIT_S_Z_MAX_PORT PINC
#define LIMIT_S_Z_MAX_BIT 1

// =============================================================================
// STEPPER MOTOR CONTROL PINS
// =============================================================================
const int MOTOR_X_CW = 27;
const int MOTOR_X_CLK = 25;

const int MOTOR_Y_CW = 29;
const int MOTOR_Y_CLK = 31;

const int MOTOR_Z_CW = 35;
const int MOTOR_Z_CLK = 37;

// =============================================================================
// STEPPER MOTOR PARAMETERS
// =============================================================================
const long MOTOR_X_SPEED = 4000;  // Microstep: 2A
const long MOTOR_Y_SPEED = 4500;    // Microstep: 16
const long MOTOR_Z_SPEED = 1000;  // Microstep: 2B

const long MOTOR_X_ACCELERATION = 40000;  // Microstep: 2A
const long MOTOR_Y_ACCELERATION = 20000;    // Microstep: 16
const long MOTOR_Z_ACCELERATION = 8000;  // Microstep: 2B

const long STEPS_PER_UNIT_X = 100L;
const long STEPS_PER_UNIT_Y = 330;
const long STEPS_PER_UNIT_Z = 90L;

// =============================================================================
// MACHINE DIMENSIONS (in mm)
// =============================================================================
// VERSION SPECIFIC: Change based on machine size
// (L) Large 300x300: TRAY_X_MAX = 400, TRAY_Y_MAX = 330
// (S) Small 200x300: TRAY_X_MAX = 190, TRAY_Y_MAX = 250
const int TRAY_X_MAX = 400;
const int TRAY_Y_MAX = 330;

// =============================================================================
// PROFILE SETTINGS
// =============================================================================
// VERSION SPECIFIC: Change based on machine size
// (L) Large 300x300: TUBES_X_MAX = 33, TUBES_Y_MAX = 33
// (S) Small 200x300: TUBES_X_MAX = 20, TUBES_Y_MAX = 27
const int TUBES_X_MAX = 42;
const int TUBES_Y_MAX = 33;
const int TUBES_X_MIN = 1;
const int TUBES_Y_MIN = 1;

const int ORIGIN_DEFAULT = 10;
const int ORIGIN_X_MIN = 0;
const int ORIGIN_Y_MIN = 0;
const int ORIGIN_X_MAX = 999;
const int ORIGIN_Y_MAX = 999;

const float Z_DIP_DEFAULT = 0.0;
const int Z_DIP_MIN = 0;
const int Z_DIP_MAX = 999;

const int PITCH_DEFAULT = 9;
const int PITCH_X_MIN = 9;
const int PITCH_X_MAX = 999;
const int PITCH_Y_MIN = 9;
const int PITCH_Y_MAX = 999;

const int CYCLES_DEFAULT = 2;
const int CYCLES_MIN = 1;
const int CYCLES_MAX = 99;

// =============================================================================
// FIRMWARE VERSION
// =============================================================================
const char FWVER[] PROGMEM = "4.0";

// =============================================================================
// FEATURE FLAGS
// =============================================================================
const bool Z_DISABLED = false;

// =============================================================================
// DEBUG SETTINGS
// =============================================================================
const int DEBUG_NO_LOG = 0; // 1 to disable logging
const int DEBUG_ONLY_SCREEN = 1; // 1 to bypass comms with the dispenser / steppers 
const int DEBUG_MEMORY_MONITOR = 0; // 1 to enable memory monitoring

const char DEBUG_MODE_KEYWORD[] PROGMEM = "debug";
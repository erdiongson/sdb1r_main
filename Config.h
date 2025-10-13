#pragma once

// =============================================================================
// LIMIT SWITCH PINS
// =============================================================================
#define LIMIT_S_X_MIN 46
#define LIMIT_S_X_MAX 22

#define LIMIT_S_Y_MIN 32
#define LIMIT_S_Y_MAX 44

#define LIMIT_S_Z_MIN 36
#define LIMIT_S_Z_MAX 38

// =============================================================================
// MOTOR CONTROL PINS
// =============================================================================
#define MOTOR_X_CW 27
#define MOTOR_X_CLK 25

#define MOTOR_Y_CW 29
#define MOTOR_Y_CLK 31

#define MOTOR_z_CW 35
#define MOTOR_Z_CLK 37

// =============================================================================
// MOTOR SPEED SETTINGS
// =============================================================================
#define MOTOR_X_SPEED 80000  // 8000, 2000
#define MOTOR_Y_SPEED 800    // 4800, 1000
#define MOTOR_Z_SPEED 80000  // 4800, 1000

// =============================================================================
// MOTOR ACCELERATION SETTINGS
// =============================================================================
#define MOTOR_X_ACCELERATION 100000  // 8000, 4800, 3200
#define MOTOR_Y_ACCELERATION 1000    // 4000, 2400, 1600, 3200
#define MOTOR_Z_ACCELERATION 100000  // 4000, 2400, 1600, 3200

// =============================================================================
// MACHINE DIMENSIONS (in mm)
// =============================================================================
// VERSION SPECIFIC: Change based on machine size
// (L) Large 300x300: TRAY_X_MAX = 400, TRAY_Y_MAX = 330
// (S) Small 200x300: TRAY_X_MAX = 190, TRAY_Y_MAX = 250
#define TRAY_X_MAX 400
#define TRAY_Y_MAX 330

// =============================================================================
// STEPS PER UNIT (Motor Calibration)
// =============================================================================
// VERSION SPECIFIC: Change based on machine size and motor configuration
//
// --- SMALL (S) VERSION ---
// #define STEPS_PER_UNIT_X         400L
// #define STEPS_PER_UNIT_Y         160L

// --- LARGE (L) VERSION --- (CURRENTLY ACTIVE)
#define STEPS_PER_UNIT_X 400L
#define STEPS_PER_UNIT_Y 160L
#define STEPS_PER_UNIT_Z 390L

// =============================================================================
// SECURITY
// =============================================================================
#define SUPER_PASSWORD "superXQ"
#define INITIAL_PASSWORD "init1234"

// =============================================================================
// DISPENSER SETTINGS
// =============================================================================
// Defines the number of "dispenses" performed during priming.
#define PRIME_DISPENSE_NUM 2

// Vibration settings
#define VIBRATION_LEVEL_DEFAULT 0
#define VIBRATION_LEVEL_MAX 4

#define VIBRATION_DURATION_DEFAULT 2
#define VIBRATION_DURATION_MIN 1
#define VIBRATION_DURATION_MAX 5

// Serial communication timeouts (in milliseconds)
#define DISPENSER_TIMEOUT_MS 5000
#define DISPENSER_POLL_DELAY_MS 10

// =============================================================================
// DEBUG SETTINGS
// =============================================================================
#define DEBUG_NO_LOG 0
#define DEBUG_ONLY_SCREEN 0

// =============================================================================
// PROFILE SETTINGS
// =============================================================================
#define EEPROM_SIZE 4096  // For an Arduino Mega
#define MAX_PROFILES 10
#define RESERVED_PROFILE_SIZE 300
#define PROFILE_START_ADDR 0
#define PASSWORD_EEPROM_ADDR 4000

// VERSION SPECIFIC: Change based on machine size
// (L) Large 300x300: TUBES_X_MAX = 33, TUBES_Y_MAX = 33
// (S) Small 200x300: TUBES_X_MAX = 20, TUBES_Y_MAX = 27
#define TUBES_X_MAX 42
#define TUBES_Y_MAX 33
#define TUBES_X_MIN 1
#define TUBES_Y_MIN 1

#define PROFILE_NAME_MAX_LEN 30
#define PASSWORD_MAX_LEN 30
#define SKIP_STRING_LEN 45

#define ORIGIN_DEFAULT 10
#define ORIGIN_X_MAX 999
#define ORIGIN_Y_MAX 999

#define Z_DIP_DEFAULT 0.0
#define Z_DIP_MIN 0
#define Z_DIP_MAX 999

#define PITCH_DEFAULT 9
#define PITCH_X_MIN 9
#define PITCH_X_MAX 999
#define PITCH_Y_MIN 9
#define PITCH_Y_MAX 999

#define CYCLES_DEFAULT 2
#define CYCLES_MIN 1
#define CYCLES_MAX 99

#define STAGGERED_OFFSET_FACTOR 0.5

// =============================================================================
// UI TIMING SETTINGS
// =============================================================================
#define DIALOG_DISPLAY_DURATION_MS 2000
#define ERROR_DISPLAY_DURATION_MS 3000
#define KEYBOARD_TRANSITION_DELAY_MS 200
#define KEYBOARD_ERROR_DISPLAY_MS 1000

#define KEYPAD_MAX_LEN 6
#define KEYBOARD_MAX_LEN 30

// =============================================================================
// MOVEMENT TEST SETTINGS
// =============================================================================
#define CM_TO_MM_MULTIPLIER 10
#define MAX_MOVE_DISTANCE_CM 99.9
#define MIN_MOVE_DISTANCE_CM 0
#define MAX_BOUNCE_COUNT 9999

#define DEFAULT_XY_DISTANCE_CM 5.0
#define DEFAULT_Z_DISTANCE_CM 3.0
#define DEFAULT_BOUNCE_COUNT 0

// =============================================================================
// FIRMWARE VERSION
// =============================================================================
#define FWVER "4.0"
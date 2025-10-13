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
// (L) Large 300x300: MAXXMM = 400, MAXYMM = 330
// (S) Small 200x300: MAXXMM = 190, MAXYMM = 250
#define MAXXMM 400
#define MAXYMM 330

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

// VERSION SPECIFIC: Change based on machine size
// (L) Large 300x300: MAX_TUBES_X = 33, MAX_TUBES_Y = 33
// (S) Small 200x300: MAX_TUBES_X = 20, MAX_TUBES_Y = 27
#define MAX_TUBES_X 42
#define MAX_TUBES_Y 33

#define PROFILE_NAME_MAX_LEN 30
#define PASSWORD_MAX_LEN 30
#define KEYPAD_MAX_LEN 6
#define ROW_COL_MAX_LEN 45

#define MAX_ORG_X 999
#define MAX_ORG_Y 999

#define MIN_Z_DIP 0
#define MAX_Z_DIP 999

#define MIN_NUM_X 1
#define MIN_NUM_Y 1

#define MIN_PITCH_X 9
#define MAX_PITCH_X 999

#define MIN_PITCH_Y 9
#define MAX_PITCH_Y 999

#define MIN_CYCLE 1
#define MAX_CYCLE 99

// =============================================================================
// FIRMWARE VERSION
// =============================================================================
#define FWVER "4.0"
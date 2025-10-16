#pragma once

// =============================================================================
// LIMIT SWITCH PINS
// =============================================================================
const int LIMIT_S_X_MIN = 46;
const int LIMIT_S_X_MAX = 22;

const int LIMIT_S_Y_MIN = 32;
const int LIMIT_S_Y_MAX = 44;

const int LIMIT_S_Z_MIN = 38;
const int LIMIT_S_Z_MAX = 36;

// =============================================================================
// MOTOR CONTROL PINS
// =============================================================================
const int MOTOR_X_CW = 27;
const int MOTOR_X_CLK = 25;

const int MOTOR_Y_CW = 29;
const int MOTOR_Y_CLK = 31;

const int MOTOR_z_CW = 35;
const int MOTOR_Z_CLK = 37;

// =============================================================================
// MOTOR SPEED SETTINGS
// =============================================================================
const long MOTOR_X_SPEED = 4000;  // Microstep: 2A
const long MOTOR_Y_SPEED = 80000;    // Microstep: 16
const long MOTOR_Z_SPEED = 1000;  // Microstep: 2B

// =============================================================================
// MOTOR ACCELERATION SETTINGS
// =============================================================================
const long MOTOR_X_ACCELERATION = 40000;  // Microstep: 2A
const long MOTOR_Y_ACCELERATION = 100000;    // Microstep: 16
const long MOTOR_Z_ACCELERATION = 8000;  // Microstep: 2B

// =============================================================================
// MACHINE DIMENSIONS (in mm)
// =============================================================================
// VERSION SPECIFIC: Change based on machine size
// (L) Large 300x300: TRAY_X_MAX = 400, TRAY_Y_MAX = 330
// (S) Small 200x300: TRAY_X_MAX = 190, TRAY_Y_MAX = 250
const int TRAY_X_MAX = 400;
const int TRAY_Y_MAX = 330;

// =============================================================================
// STEPS PER UNIT (Motor Calibration)
// =============================================================================
// VERSION SPECIFIC: Change based on machine size and motor configuration
//
// --- SMALL (S) VERSION ---
// const long STEPS_PER_UNIT_X = 400L;
// const long STEPS_PER_UNIT_Y = 160L;

// --- LARGE (L) VERSION --- (CURRENTLY ACTIVE)
// const long STEPS_PER_UNIT_X = 400L;
const long STEPS_PER_UNIT_X = 100L;
const long STEPS_PER_UNIT_Y = 100L;
const long STEPS_PER_UNIT_Z = 450L;

// =============================================================================
// SECURITY
// =============================================================================
const char* const SUPER_PASSWORD = "superXQ";
const char* const INITIAL_PASSWORD = "init1234";

// =============================================================================
// DISPENSER SETTINGS
// =============================================================================
// Defines the number of "dispenses" performed during priming.
const int PRIME_DISPENSE_NUM = 2;

// Vibration settings
const int VIBRATION_LEVEL_DEFAULT = 0;
const int VIBRATION_LEVEL_MAX = 4;

const int VIBRATION_DURATION_DEFAULT = 2;
const int VIBRATION_DURATION_MIN = 1;
const int VIBRATION_DURATION_MAX = 5;

// Serial communication timeouts (in milliseconds)
const unsigned long DISPENSER_ACK_TIMEOUT_MS = 5000;
const unsigned long DISPENSER_CYCLE_TIMEOUT_MS = 30000;
const unsigned long DISPENSER_POLL_DELAY_MS = 10;

// =============================================================================
// DEBUG SETTINGS
// =============================================================================
const int DEBUG_NO_LOG = 0; // 1 to disable logging
const int DEBUG_ONLY_SCREEN = 0; // 1 to bypass comms with the dispenser / steppers 
const int DEBUG_MEMORY_MONITOR = 0; // 1 to enable memory monitoring

const char* const DEBUG_MODE_KEYWORD = "debug";

// =============================================================================
// PROFILE SETTINGS
// =============================================================================
const int EEPROM_SIZE = 4096;  // For an Arduino Mega
const int MAX_PROFILES = 10;
const int RESERVED_PROFILE_SIZE = 300;
const int PROFILE_START_ADDR = 0;
const int PASSWORD_EEPROM_ADDR = 4000;

// VERSION SPECIFIC: Change based on machine size
// (L) Large 300x300: TUBES_X_MAX = 33, TUBES_Y_MAX = 33
// (S) Small 200x300: TUBES_X_MAX = 20, TUBES_Y_MAX = 27
const int TUBES_X_MAX = 42;
const int TUBES_Y_MAX = 33;
const int TUBES_X_MIN = 1;
const int TUBES_Y_MIN = 1;

const int PROFILE_NAME_MAX_LEN = 30;
const int PASSWORD_MAX_LEN = 30;
const int SKIP_STRING_LEN = 45;
const int SKIP_STRING_INDIVIDUAL_LEN = 200;
const int MAX_SKIP_POSITIONS = 2;  // 180 bytes / 2 bytes per position (same memory as 3x60 char arrays)

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

const float STAGGERED_OFFSET_FACTOR = 0.5;

// =============================================================================
// UI TIMING SETTINGS
// =============================================================================
const unsigned long DIALOG_DISPLAY_DURATION_MS = 2000;
const unsigned long ERROR_DISPLAY_DURATION_MS = 3000;
const unsigned long KEYBOARD_TRANSITION_DELAY_MS = 200;
const unsigned long KEYBOARD_ERROR_DISPLAY_MS = 1000;

// =============================================================================
// TEST / PREVIEW SETTINGS
// =============================================================================
const int CM_TO_MM_MULTIPLIER = 10;
const float MAX_MOVE_DISTANCE_CM = 99.9;
const float MIN_MOVE_DISTANCE_CM = 0;

const int MIN_BOUNCE_COUNT = 0;
const int MAX_BOUNCE_COUNT = 9999;

const float DEFAULT_XY_DISTANCE_CM = 5.0;
const float DEFAULT_Z_DISTANCE_CM = 3.0;
const int DEFAULT_BOUNCE_COUNT = 0;

const int MIN_DISPENSE_REPEAT_COUNT = 0;
const int MAX_DISPENSE_REPEAT_COUNT = 9999;
const int DEFAULT_DISPENSE_REPEAT_COUNT = 0;

const unsigned long MOVE_TEST_REFRESH_INTERVAL_MS = 1000;
const unsigned long DISPENSE_TEST_REFRESH_INTERVAL_MS = 1000;
const unsigned long SIMULATION_UPDATE_INTERVAL_MS = 600;


// =============================================================================
// FIRMWARE VERSION
// =============================================================================
const char* const FWVER = "4.0";

// =============================================================================
// Others
// =============================================================================
const bool Z_DISABLED = false;
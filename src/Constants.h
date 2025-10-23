#pragma once

// =============================================================================
// KEYBOARD SETTINGS
// =============================================================================
const int KEYBOARD_FONT = 27;
const int KEYBOARD_MAX_PER_LINE = 25;
const int KEYBOARD_LINE_HEIGHT = 22;
const int KEYBOARD_VISIBLE_LINES = 3;  // Lines shown at once
const int KEYBOARD_MAX_LINES = 9;     // Maximum total lines

const int KEYBOARD_MAX_LEN = (KEYBOARD_MAX_PER_LINE * KEYBOARD_MAX_LINES);

const int KEYPAD_MAX_LEN = 10;

// =============================================================================
// AXIS STATES
// =============================================================================
const int AXIS_STATE_COMPLETE = 0;
const int AXIS_STATE_RUNNING = 1;
const int AXIS_STATE_ERROR_LIMIT_SWITCH = 2;

// =============================================================================
// DISPENSER STATES
// =============================================================================
const int DISPENSER_STATE_IDLING = 0;
const int DISPENSER_STATE_SENT = 1;
const int DISPENSER_STATE_ACKNOWLEDGED = 2;
const int DISPENSER_STATE_ERROR_ACK_ERROR = 3;
const int DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE = 4;
const int DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED = 5;
const int DISPENSER_STATE_ERROR_CYCLES_TIMEOUT = 6;
const int DISPENSER_STATE_BLOCKED = 7;

// =============================================================================
// CONTROLLER IDENTIFIERS
// =============================================================================
const int CONTROLLER_STARTUP = 0;
const int CONTROLLER_HOMING = 1;
const int CONTROLLER_READY = 2;
const int CONTROLLER_RUN = 3;
const int CONTROLLER_MOVE_TEST = 4;
const int CONTROLLER_DISPENSE_TEST = 5;
const int CONTROLLER_SETTINGS = 6;
const int CONTROLLER_PROFILE = 7;
const int CONTROLLER_PREVIEW = 8;
const int CONTROLLER_DEBUG = 9;
const int CONTROLLER_ADVANCED_SETTINGS = 10;
const int CONTROLLER_STEPPER_TEST = 11;

// =============================================================================
// UI TAG CONSTANTS - Main Screen (Ready, Run, Homing)
// =============================================================================
const int TAG_SETTING = 2;
const int TAG_START = 3;
const int TAG_PAUSE = 4;
const int TAG_STOP = 5;
const int TAG_CONTINUE = 101;

// =============================================================================
// UI TAG CONSTANTS - Profile Management
// =============================================================================
const int TAG_PROFILE_BACK = 6;
const int TAG_PROFILE_LOAD = 7;
const int TAG_ADVANCED = 8;
const int TAG_PROFILE_UP = 9;
const int TAG_PROFILE_DOWN = 10;
const int TAG_PROFILE_CHANGE_PASSWORD = 40;

// =============================================================================
// UI TAG CONSTANTS - Configuration
// =============================================================================
const int TAG_CONFIG_HOME = 102;
const int TAG_CONFIG_LOAD = 11;
const int TAG_CONFIG_SAVE = 12;
const int TAG_CONFIG_PROFILE_NAME = 13;
const int TAG_CONFIG_TUBES_X = 14;
const int TAG_CONFIG_TUBES_Y = 15;
const int TAG_CONFIG_PITCH_X = 16;
const int TAG_CONFIG_PITCH_Y = 17;
const int TAG_CONFIG_ORIGIN_X = 18;
const int TAG_CONFIG_ORIGIN_Y = 19;
const int TAG_CONFIG_PREVIEW = 36;
const int TAG_CONFIG_PREVIEW_BACK = 37;

// =============================================================================
// UI TAG CONSTANTS - Advanced Settings
// =============================================================================
const int TAG_NUM_CYCLE = 20;
const int TAG_Z_DIP = 21;
const int TAG_VIBRATION_LEVEL = 28;
const int TAG_PASSWORD_ENABLED = 29;
const int TAG_VIBRATION_DURATION = 30;
const int TAG_SKIP_COLUMNS = 31;
const int TAG_SKIP_ROWS = 32;
const int TAG_SKIP_SINGLE_POS = 33;
const int TAG_ADV_PROF_BACK = 34;
const int TAG_STAGGERED_TOGGLE = 38;

// =============================================================================
// UI TAG CONSTANTS - Movement Control
// =============================================================================
const int TAG_MOVE_UP = 50;
const int TAG_MOVE_DOWN = 51;
const int TAG_MOVE_LEFT = 52;
const int TAG_MOVE_RIGHT = 53;
const int TAG_Z_UP = 54;
const int TAG_Z_DOWN = 55;
const int TAG_MOVE_BACK = 56;
const int TAG_MOVE_XY_DIST = 57;
const int TAG_MOVE_Z_DIST = 58;
const int TAG_MOVE_BOUNCE = 59;
const int TAG_MOVE_STOP = 100;
const int TAG_MOVE_BLOCKING = 103;

// =============================================================================
// UI TAG CONSTANTS - Dispense Test Control
// =============================================================================
const int TAG_DISPENSE = 60;
const int TAG_VIB_U0 = 61;
const int TAG_VIB_U1 = 62;
const int TAG_VIB_U2 = 63;
const int TAG_VIB_U3 = 64;
const int TAG_VIB_U4 = 65;
const int TAG_VIB_TIME_1 = 66;
const int TAG_VIB_TIME_2 = 67;
const int TAG_VIB_TIME_3 = 68;
const int TAG_VIB_TIME_4 = 69;
const int TAG_VIB_TIME_5 = 70;
const int TAG_DISPENSE_BACK = 71;
const int TAG_DISPENSE_REPEAT = 72;
const int TAG_DISPENSE_STOP = 73;

// =============================================================================
// UI TAG CONSTANTS - Preview Screen
// =============================================================================
const int TAG_PREVIEW_SIMULATE = 80;
const int TAG_PREVIEW_STOP = 81;

// =============================================================================
// UI TAG CONSTANTS - Debug Screen
// =============================================================================
const int TAG_DEBUG_MOVE_TEST = 90;
const int TAG_DEBUG_DISPENSER_TEST = 91;
const int TAG_DEBUG_BLANK_EEPROM = 92;
const int TAG_DEBUG_RESET_PROFILES = 93;
const int TAG_DEBUG_BACK = 94;
const int TAG_DEBUG_DIALOG_TEST = 95;
const int TAG_DEBUG_STEPPER_TEST = 96;

// =============================================================================
// UI TAG CONSTANTS - Stepper Test Control
// =============================================================================
const int TAG_STEPPER_AXIS_X = 110;
const int TAG_STEPPER_AXIS_Y = 111;
const int TAG_STEPPER_AXIS_Z = 112;
const int TAG_STEPPER_SPEED_VALUE = 113;
const int TAG_STEPPER_SPEED_MINUS_10K = 114;
const int TAG_STEPPER_SPEED_PLUS_10K = 115;
const int TAG_STEPPER_SPEED_MINUS_1K = 116;
const int TAG_STEPPER_SPEED_MINUS_100 = 117;
const int TAG_STEPPER_SPEED_PLUS_100 = 118;
const int TAG_STEPPER_SPEED_PLUS_1K = 119;
const int TAG_STEPPER_SPEED_RESET = 120;
const int TAG_STEPPER_ACCEL_VALUE = 121;
const int TAG_STEPPER_ACCEL_MINUS_10K = 122;
const int TAG_STEPPER_ACCEL_PLUS_10K = 123;
const int TAG_STEPPER_ACCEL_MINUS_1K = 124;
const int TAG_STEPPER_ACCEL_MINUS_100 = 125;
const int TAG_STEPPER_ACCEL_PLUS_100 = 126;
const int TAG_STEPPER_ACCEL_PLUS_1K = 127;
const int TAG_STEPPER_ACCEL_RESET = 128;
const int TAG_STEPPER_MOVE_MINUS = 129;
const int TAG_STEPPER_MOVE_PLUS = 130;
const int TAG_STEPPER_MOVE_AMOUNT = 131;
const int TAG_STEPPER_BACK = 132;
const int TAG_STEPPER_BLOCKING = 133;
const int TAG_STEPPER_STOP = 134;

// =============================================================================
// INTERACTION INTERVALS (in milliseconds)
// =============================================================================
const int INTERACT_INTERVAL_AXIS_IDLE = 20;
const int INTERACT_INTERVAL_AXIS_RUNNING = 100;

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Message Structure
// =============================================================================
const int MSG_SOT = 0;      // Start of Transmission position
const int MSG_COMMAND = 1;  // Command/status byte position
const int MSG_DATA1 = 2;    // Data byte 1 position
const int MSG_DATA2 = 3;    // Data byte 2 (checksum) position
const int MSG_EOT = 4;      // End of Transmission position
const int MSG_LENGTH = 5;   // Total message length

const uint8_t DISPENSER_START_BYTE = 0xEF;
const uint8_t DISPENSER_END_BYTE = 0xFE;

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Command Values
// =============================================================================
const uint8_t SDB_HANDSHAKE = 0x06;       // Handshake command
const uint8_t SDB_VIBRATE_LEVEL = 0x54;   // Command to set vibration mode ON
const uint8_t SDB_VIBRATE_TIME = 0x65;    // Command to set vibration time
const uint8_t SDB_DISPENSE_START = 0x46;  // Command to start dispensing

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Response Values
// =============================================================================
const uint16_t ACKNOWLEDGE = 0x46;           // Acknowledgment response
const uint16_t DISPENSE_DONE = 0xF9;         // Dispense operation completed
const uint16_t IR_SENSOR_FAILURE = 0xE1;     // Error: IR sensor failure
const uint16_t MARKER_NOT_DETECTED = 0xE2;   // Error: Marker not detected
const uint16_t ACK_ERROR = 0xE0;             // Error: Acknowledgment error
const uint16_t CYCLE_TIMEOUT_ERROR = 0xE10;  // Error: Cycle timeout

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Vibration Settings
// =============================================================================
const uint8_t VIBMODE_U0 = 0x71;  // Former 0xC2, changed for 1RC
const uint8_t VIBDUR_1 = 0x81;    // 1 second/s

// =============================================================================
// DISPENSER SETTINGS
// =============================================================================
// Vibration settings
const int VIBRATION_LEVEL_DEFAULT = 0;
const int VIBRATION_LEVEL_MAX = 4;

const int VIBRATION_DURATION_DEFAULT = 2;
const int VIBRATION_DURATION_MIN = 1;
const int VIBRATION_DURATION_MAX = 5;

// =============================================================================
// PLC SERIAL PROTOCOL
// =============================================================================
const uint8_t PLC_START_BYTE = 0xEF;
const uint8_t PLC_END_BYTE = 0xFE;
const int PLC_MESSAGE_LENGTH = 5;

// PLC Command Codes
const uint8_t PLC_CMD_START = 0x30;
const uint8_t PLC_CMD_STOP = 0x31;
const uint8_t PLC_CMD_PAUSE = 0x32;
const uint8_t PLC_CMD_RAISE_Z = 0x42;
const uint8_t PLC_CMD_LOWER_Z = 0x41;

// PLC Response Codes
const uint8_t PLC_BUSY_BYTE = 0x16;  // Busy response byte
const uint8_t PLC_NAK_BYTE = 0x15;   // Negative acknowledgment byte

// =============================================================================
// PROFILE SETTINGS
// =============================================================================
const int EEPROM_SIZE = 4096;  // For an Arduino Mega
const int MAX_PROFILES = 10;
const int RESERVED_PROFILE_SIZE = 300;
const int PROFILE_START_ADDR = 0;
const int PASSWORD_EEPROM_ADDR = 4000;

const int PROFILE_NAME_MAX_LEN = 30;
const int PASSWORD_MAX_LEN = 30;

const int SKIP_STRING_LEN = 200;
const int SKIP_STRING_INDIVIDUAL_LEN = 210;

const int MAX_SKIP_POSITIONS_ROWS = 33;
const int MAX_SKIP_POSITIONS_COLUMNS = 42;
const int MAX_SKIP_POSITIONS_INDIVIDUAL = 30;
const int MAX_SKIP_POSITIONS_TOTAL = 100;

const float STAGGERED_OFFSET_FACTOR = 0.5;

// =============================================================================
// TEST / PREVIEW SETTINGS
// =============================================================================
const int CM_TO_MM_MULTIPLIER = 10;
const float MAX_MOVE_DISTANCE_CM = 99.9;
const float MIN_MOVE_DISTANCE_CM = 0;

const int MIN_BOUNCE_COUNT = 0;
const int MAX_BOUNCE_COUNT = 9999;

const float DEFAULT_XY_DISTANCE_CM = 5.0;
const float DEFAULT_Z_DISTANCE_CM = 1.0;
const int DEFAULT_BOUNCE_COUNT = 0;

const int MIN_DISPENSE_REPEAT_COUNT = 0;
const int MAX_DISPENSE_REPEAT_COUNT = 9999;
const int DEFAULT_DISPENSE_REPEAT_COUNT = 0;

const unsigned long MOVE_TEST_REFRESH_INTERVAL_MS = 500;
const unsigned long DISPENSE_TEST_REFRESH_INTERVAL_MS = 1000;
const unsigned long SIMULATION_UPDATE_INTERVAL_MS = 600;

// =============================================================================
// DIALOG CODES
// =============================================================================
const int DIALOG_ERROR_IR_SENSOR = 2;
const int DIALOG_ERROR_MARKER_NOT_DETECTED = 3;
const int DIALOG_ERROR_LIMIT_SWITCH = 4;
const int DIALOG_ERROR_LIMIT_SWITCH_HOMING = 5;
const int DIALOG_ERROR_ACK_ERROR = 6;
const int DIALOG_PROFILE_SAVED = 7;
const int DIALOG_PROFILE_LOADED = 8;
const int DIALOG_ERROR_PASSWORD_MISMATCH = 9;
const int DIALOG_PASSWORD_CHANGED = 10;
const int DIALOG_ERROR_WRONG_PASSWORD = 11;
const int DIALOG_ERROR_DIMENSION = 12;
const int DIALOG_ERROR_CYCLE_TIMEOUT = 13;
const int DIALOG_ERROR_PARAMETER = 14;
const int DIALOG_ERROR_SKIP_VALUES = 15;
const int DIALOG_ERROR_SKIP_COUNT_EXCEEDED = 16;
const int DIALOG_ERROR_NO_VALID_POSITIONS = 17;

// =============================================================================
// MODEL IDENTIFIERS AND MACROS
// =============================================================================
#define MODEL_S 1
#define MODEL_M 2
#define MODEL_L 3

// Stringify macros to convert preprocessor tokens to strings
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
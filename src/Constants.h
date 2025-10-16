#pragma once

// =============================================================================
// AXIS STATES
// =============================================================================
#define AXIS_STATE_COMPLETE 0
#define AXIS_STATE_RUNNING 1
#define AXIS_STATE_ERROR_LIMIT_SWITCH 2

// =============================================================================
// DISPENSER STATES
// =============================================================================
#define DISPENSER_STATE_IDLING 0
#define DISPENSER_STATE_SENT 1
#define DISPENSER_STATE_ACKNOWLEDGED 2
#define DISPENSER_STATE_ERROR_ACK_ERROR 3
#define DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE 4
#define DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED 5
#define DISPENSER_STATE_ERROR_CYCLES_TIMEOUT 6
#define DISPENSER_STATE_BLOCKED 7

// =============================================================================
// CONTROLLER IDENTIFIERS
// =============================================================================
#define CONTROLLER_STARTUP 0
#define CONTROLLER_HOMING 1
#define CONTROLLER_READY 2
#define CONTROLLER_RUN 3
#define CONTROLLER_MOVE_TEST 4
#define CONTROLLER_DISPENSE_TEST 5
#define CONTROLLER_SETTINGS 6
#define CONTROLLER_PROFILE 7
#define CONTROLLER_PREVIEW 8
#define CONTROLLER_DEBUG 9
#define CONTROLLER_ADVANCED_SETTINGS 10
#define CONTROLLER_STEPPER_TEST 11

// =============================================================================
// UI TAG CONSTANTS - Main Screen (Ready, Run, Homing)
// =============================================================================
#define TAG_SETTING 2
#define TAG_START 3
#define TAG_PAUSE 4
#define TAG_STOP 5
#define TAG_CONTINUE 101

// =============================================================================
// UI TAG CONSTANTS - Profile Management
// =============================================================================
#define TAG_PROFILE_BACK 6
#define TAG_PROFILE_LOAD 7
#define TAG_ADVANCED 8
#define TAG_PROFILE_UP 9
#define TAG_PROFILE_DOWN 10
#define TAG_PROFILE_CHANGE_PASSWORD 40

// =============================================================================
// UI TAG CONSTANTS - Configuration
// =============================================================================
#define TAG_CONFIG_HOME 102
#define TAG_CONFIG_LOAD 11
#define TAG_CONFIG_SAVE 12
#define TAG_CONFIG_PROFILE_NAME 13
#define TAG_CONFIG_TUBES_X 14
#define TAG_CONFIG_TUBES_Y 15
#define TAG_CONFIG_PITCH_X 16
#define TAG_CONFIG_PITCH_Y 17
#define TAG_CONFIG_ORIGIN_X 18
#define TAG_CONFIG_ORIGIN_Y 19
#define TAG_CONFIG_PREVIEW 36
#define TAG_CONFIG_PREVIEW_BACK 37

// =============================================================================
// UI TAG CONSTANTS - Advanced Settings
// =============================================================================
#define TAG_NUM_CYCLE 20
#define TAG_Z_DIP 21
#define TAG_VIBRATION_LEVEL 28
#define TAG_PASSWORD_ENABLED 29
#define TAG_VIBRATION_DURATION 30
#define TAG_SKIP_COLUMNS 31
#define TAG_SKIP_ROWS 32
#define TAG_SKIP_SINGLE_POS 33
#define TAG_ADV_PROF_BACK 34
#define TAG_STAGGERED_TOGGLE 38

// =============================================================================
// UI TAG CONSTANTS - Movement Control
// =============================================================================
#define TAG_MOVE_UP 50
#define TAG_MOVE_DOWN 51
#define TAG_MOVE_LEFT 52
#define TAG_MOVE_RIGHT 53
#define TAG_Z_UP 54
#define TAG_Z_DOWN 55
#define TAG_MOVE_BACK 56
#define TAG_MOVE_XY_DIST 57
#define TAG_MOVE_Z_DIST 58
#define TAG_MOVE_BOUNCE 59
#define TAG_MOVE_STOP 100

// =============================================================================
// UI TAG CONSTANTS - Dispense Test Control
// =============================================================================
#define TAG_DISPENSE 60
#define TAG_VIB_U0 61
#define TAG_VIB_U1 62
#define TAG_VIB_U2 63
#define TAG_VIB_U3 64
#define TAG_VIB_U4 65
#define TAG_VIB_TIME_1 66
#define TAG_VIB_TIME_2 67
#define TAG_VIB_TIME_3 68
#define TAG_VIB_TIME_4 69
#define TAG_VIB_TIME_5 70
#define TAG_DISPENSE_BACK 71
#define TAG_DISPENSE_REPEAT 72
#define TAG_DISPENSE_STOP 73

// =============================================================================
// UI TAG CONSTANTS - Preview Screen
// =============================================================================
#define TAG_PREVIEW_SIMULATE 80
#define TAG_PREVIEW_STOP 81

// =============================================================================
// UI TAG CONSTANTS - Debug Screen
// =============================================================================
#define TAG_DEBUG_MOVE_TEST 90
#define TAG_DEBUG_DISPENSER_TEST 91
#define TAG_DEBUG_BLANK_EEPROM 92
#define TAG_DEBUG_RESET_PROFILES 93
#define TAG_DEBUG_BACK 94
#define TAG_DEBUG_DIALOG_TEST 95
#define TAG_DEBUG_STEPPER_TEST 96

// =============================================================================
// UI TAG CONSTANTS - Stepper Test Control
// =============================================================================
#define TAG_STEPPER_AXIS_X 110
#define TAG_STEPPER_AXIS_Y 111
#define TAG_STEPPER_AXIS_Z 112
#define TAG_STEPPER_SPEED_VALUE 113
#define TAG_STEPPER_SPEED_MINUS_10K 114
#define TAG_STEPPER_SPEED_PLUS_10K 115
#define TAG_STEPPER_SPEED_MINUS_1K 116
#define TAG_STEPPER_SPEED_MINUS_100 117
#define TAG_STEPPER_SPEED_PLUS_100 118
#define TAG_STEPPER_SPEED_PLUS_1K 119
#define TAG_STEPPER_SPEED_RESET 120
#define TAG_STEPPER_ACCEL_VALUE 121
#define TAG_STEPPER_ACCEL_MINUS_10K 122
#define TAG_STEPPER_ACCEL_PLUS_10K 123
#define TAG_STEPPER_ACCEL_MINUS_1K 124
#define TAG_STEPPER_ACCEL_MINUS_100 125
#define TAG_STEPPER_ACCEL_PLUS_100 126
#define TAG_STEPPER_ACCEL_PLUS_1K 127
#define TAG_STEPPER_ACCEL_RESET 128
#define TAG_STEPPER_MOVE_MINUS 129
#define TAG_STEPPER_MOVE_PLUS 130
#define TAG_STEPPER_MOVE_AMOUNT 131
#define TAG_STEPPER_BACK 132

// =============================================================================
// INTERACTION INTERVALS (in milliseconds)
// =============================================================================
#define INTERACT_INTERVAL_AXIS_IDLE 20
#define INTERACT_INTERVAL_AXIS_RUNNING 200

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Message Structure
// =============================================================================
#define MSG_SOT 0      // Start of Transmission position
#define MSG_COMMAND 1  // Command/status byte position
#define MSG_DATA1 2    // Data byte 1 position
#define MSG_DATA2 3    // Data byte 2 (checksum) position
#define MSG_EOT 4      // End of Transmission position
#define MSG_LENGTH 5   // Total message length

#define DISPENSER_START_BYTE 0xEF
#define DISPENSER_END_BYTE 0xFE

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Command Values
// =============================================================================
#define SDB_HANDSHAKE 0x06       // Handshake command
#define SDB_VIBRATE_LEVEL 0x54   // Command to set vibration mode ON
#define SDB_VIBRATE_TIME 0x65    // Command to set vibration time
#define SDB_DISPENSE_START 0x46  // Command to start dispensing

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Response Values
// =============================================================================
#define ACKNOWLEDGE 0x46          // Acknowledgment response
#define DISPENSE_DONE 0xF9        // Dispense operation completed
#define IR_SENSOR_FAILURE 0xE1    // Error: IR sensor failure
#define MARKER_NOT_DETECTED 0xE2  // Error: Marker not detected
#define ACK_ERROR 0xE0            // Error: Acknowledgment error
#define CYCLE_TIMEOUT_ERROR 0xE10 // Error: Cycle timeout

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Vibration Settings
// =============================================================================
#define VIBMODE_U0 0x71  // Former 0xC2, changed for 1RC
#define VIBDUR_1 0x81    // 1 second/s

// =============================================================================
// PLC SERIAL PROTOCOL
// =============================================================================
#define PLC_START_BYTE 0xEF
#define PLC_END_BYTE 0xFE
#define PLC_MESSAGE_LENGTH 5

// PLC Command Codes
#define PLC_CMD_START 0x30
#define PLC_CMD_STOP 0x31
#define PLC_CMD_PAUSE 0x32
#define PLC_CMD_RAISE_Z 0x42
#define PLC_CMD_LOWER_Z 0x41

// PLC Response Codes
#define PLC_NAK_BYTE 0x16  // Negative acknowledgment byte

// =============================================================================
// DIALOG CODES
// =============================================================================
#define DIALOG_ERROR_IR_SENSOR 2
#define DIALOG_ERROR_MARKER_NOT_DETECTED 3
#define DIALOG_ERROR_LIMIT_SWITCH 4
#define DIALOG_ERROR_LIMIT_SWITCH_HOMING 5
#define DIALOG_ERROR_ACK_ERROR 6
#define DIALOG_PROFILE_SAVED 7
#define DIALOG_PROFILE_LOADED 8
#define DIALOG_ERROR_PASSWORD_MISMATCH 9
#define DIALOG_PASSWORD_CHANGED 10
#define DIALOG_ERROR_WRONG_PASSWORD 11
#define DIALOG_ERROR_DIMENSION 12
#define DIALOG_ERROR_CYCLE_TIMEOUT 13
#define DIALOG_ERROR_PARAMETER 14
#define DIALOG_ERROR_SKIP_VALUES 15

// =============================================================================
// ARRAY LIMITS
// =============================================================================
#define MAX_POSITIONS 100
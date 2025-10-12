#pragma once

// =============================================================================
// AXIS STATES
// =============================================================================
#define AXIS_STATE_COMPLETE                     0
#define AXIS_STATE_RUNNING                      1
#define AXIS_STATE_ERROR_LIMIT_SWITCH           2

// =============================================================================
// DISPENSER STATES
// =============================================================================
#define DISPENSER_STATE_IDLING                  0
#define DISPENSER_STATE_SENT                    1
#define DISPENSER_STATE_ACKNOWLEDGED            2
#define DISPENSER_STATE_ERROR_ACK_ERROR         3
#define DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE 4
#define DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED 5
#define DISPENSER_STATE_BLOCKED                 6

// =============================================================================
// CONTROLLER STATES
// =============================================================================
#define CONTROLLER_STARTUP                      0
#define CONTROLLER_HOMING                       1
#define CONTROLLER_READY                        2
#define CONTROLLER_RUN                          3
#define CONTROLLER_MOVE_TEST                    4
#define CONTROLLER_DISPENSE_TEST                5
#define CONTROLLER_SETTINGS                     6
#define CONTROLLER_PROFILE                      7
#define CONTROLLER_PREVIEW                      8
#define CONTROLLER_DEBUG                        9

// =============================================================================
// SCREEN/MENU STATES
// =============================================================================
#define LOGO                                    0
#define MAINMENU                                0
#define RUNMENU                                 1
#define PAUSEMENU                               2
#define HOMINGMENU                              3
#define STOPPINGMENU                            4

#define SETTING                                 2
#define START                                   3
#define PAUSE                                   4
#define STOP                                    5
#define ERROR                                   6

// =============================================================================
// UI TAG CONSTANTS - General
// =============================================================================
#define TAG_CONTINUE                            5
#define TAG_CONFIG_HOME                         6

// =============================================================================
// UI TAG CONSTANTS - Profile Management
// =============================================================================
#define TAG_PROFILE_BACK                        6
#define TAG_PROFILE_LOAD                        7
#define TAG_ADVANCED                            8
#define TAG_PROFILE_UP                          9
#define TAG_PROFILE_DOWN                        10
#define TAG_PROFILE_CHANGE_PASSWORD             40

// =============================================================================
// UI TAG CONSTANTS - Configuration
// =============================================================================
#define TAG_CONFIG_LOAD                         11
#define TAG_CONFIG_SAVE                         12
#define TAG_CONFIG_PROFILE_NAME                 13
#define TAG_CONFIG_TUBES_X                      14
#define TAG_CONFIG_TUBES_Y                      15
#define TAG_CONFIG_PITCH_X                      16
#define TAG_CONFIG_PITCH_Y                      17
#define TAG_CONFIG_ORIGIN_X                     18
#define TAG_CONFIG_ORIGIN_Y                     19
#define TAG_CONFIG_PREVIEW                      36
#define TAG_CONFIG_PREVIEW_BACK                 37

// =============================================================================
// UI TAG CONSTANTS - Advanced Settings
// =============================================================================
#define TAG_NUM_CYCLE                           20
#define TAG_Z_DIP                               21
#define TAG_VIBRATION_LEVEL                     28
#define TAG_PASSWORD_ENABLED                    29
#define TAG_VIBRATION_DURATION                  30
#define TAG_SKIP_COLUMNS                        31
#define TAG_SKIP_ROWS                           32
#define TAG_SKIP_SINGLE_POS                     33
#define TAG_ADV_PROF_BACK                       34
#define TAG_STAGGERED_TOGGLE                    38

// =============================================================================
// UI TAG CONSTANTS - Movement Control
// =============================================================================
#define TAG_MOVE_UP                             50
#define TAG_MOVE_DOWN                           51
#define TAG_MOVE_LEFT                           52
#define TAG_MOVE_RIGHT                          53
#define TAG_Z_UP                                54
#define TAG_Z_DOWN                              55
#define TAG_MOVE_BACK                           56
#define TAG_MOVE_XY_DIST                        57
#define TAG_MOVE_Z_DIST                         58
#define TAG_MOVE_BOUNCE                         59
#define TAG_MOVE_STOP                           100

// =============================================================================
// UI TAG CONSTANTS - Dispense Test Control
// =============================================================================
#define TAG_DISPENSE                            60
#define TAG_VIB_U0                              61
#define TAG_VIB_U1                              62
#define TAG_VIB_U2                              63
#define TAG_VIB_U3                              64
#define TAG_VIB_U4                              65
#define TAG_VIB_TIME_1                          66
#define TAG_VIB_TIME_2                          67
#define TAG_VIB_TIME_3                          68
#define TAG_VIB_TIME_4                          69
#define TAG_VIB_TIME_5                          70
#define TAG_DISPENSE_BACK                       71

// =============================================================================
// UI TAG CONSTANTS - Preview Screen
// =============================================================================
#define TAG_PREVIEW_SIMULATE                    80
#define TAG_PREVIEW_STOP                        81

// =============================================================================
// UI TAG CONSTANTS - Debug Screen
// =============================================================================
#define TAG_DEBUG_MOVE_TEST                     90
#define TAG_DEBUG_DISPENSER_TEST                91
#define TAG_DEBUG_BLANK_EEPROM                  92
#define TAG_DEBUG_RESET_PROFILES                93
#define TAG_DEBUG_BACK                          94
#define TAG_DEBUG_DIALOG_TEST                   95

// =============================================================================
// INTERACTION INTERVALS (in milliseconds)
// =============================================================================
#define INTERACT_INTERVAL_AXIS_IDLE             20
#define INTERACT_INTERVAL_AXIS_RUNNING          200

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Message Structure
// =============================================================================
#define MSG_SOT                                 0   // Start of Transmission position
#define MSG_COMMAND                             1   // Command/status byte position
#define MSG_DATA1                               2   // Data byte 1 position
#define MSG_DATA2                               3   // Data byte 2 (checksum) position
#define MSG_EOT                                 4   // End of Transmission position
#define MSG_LENGTH                              5   // Total message length

#define DISPENSER_START_BYTE                    0xEF
#define DISPENSER_END_BYTE                      0xFE

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Command Values
// =============================================================================
#define SDB_HANDSHAKE                           0x06    // Handshake command
#define SDB_VIBRATE_LEVEL                       0x54    // Command to set vibration mode ON
#define SDB_VIBRATE_TIME                        0x65    // Command to set vibration time
#define SDB_DISPENSE_START                      0x46    // Command to start dispensing

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Response Values
// =============================================================================
#define ACKNOWLEDGE                             0x46    // Acknowledgment response
#define DISPENSE_DONE                           0xF9    // Dispense operation completed
#define IR_SENSOR_FAILURE                       0xE1    // Error: IR sensor failure
#define MARKER_NOT_DETECTED                     0xE2    // Error: Marker not detected
#define ACK_ERROR                               0xE0    // Error: Acknowledgment error

// =============================================================================
// DISPENSER SERIAL PROTOCOL - Vibration Settings
// =============================================================================
#define VIBMODE_U0                              0x71    // Former 0xC2, changed for 1RC
#define VIBDUR_1                                0x81    // 1 second/s

// =============================================================================
// PLC SERIAL PROTOCOL
// =============================================================================
#define PLC_START_BYTE                          0xEF
#define PLC_END_BYTE                            0xFE
#define PLC_MESSAGE_LENGTH                      5

// =============================================================================
// DIALOG CODES
// =============================================================================
#define DIALOG_ERROR_IR_SENSOR                  2
#define DIALOG_ERROR_MARKER_NOT_DETECTED        3
#define DIALOG_ERROR_LIMIT_SWITCH               4
#define DIALOG_ERROR_LIMIT_SWITCH_HOMING        5
#define DIALOG_ERROR_ACK_ERROR                  6
#define DIALOG_PROFILE_SAVED                    7
#define DIALOG_PROFILE_LOADED                   8
#define DIALOG_ERROR_PASSWORD_MISMATCH          9
#define DIALOG_PASSWORD_CHANGED                 10
#define DIALOG_ERROR_WRONG_PASSWORD             11

// =============================================================================
// ARRAY LIMITS
// =============================================================================
#define MAX_POSITIONS                           100
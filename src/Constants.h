#define AXIS_STATE_COMPLETE 0
#define AXIS_STATE_RUNNING 1
#define AXIS_STATE_ERROR_LIMIT_SWITCH 2

#define DISPENSER_STATE_BLOCKED 6
#define DISPENSER_STATE_IDLING 0
#define DISPENSER_STATE_SENT 1
#define DISPENSER_STATE_ACKNOWLEDGED 2
#define DISPENSER_STATE_ERROR_ACK_ERROR 3
#define DISPENSER_STATE_ERROR_IR_SENSOR_FAILURE 4
#define DISPENSER_STATE_ERROR_MARKER_NOT_DETECTED 5

#define STOP                 5
#define START                3
#define PAUSE                4
#define ERROR                6
#define SETTING              2
#define LOGO                 0
#define MAINMENU             0
#define RUNMENU              1
#define PAUSEMENU            2
#define TXRXERROR            3
#define HOMEERROR            4

#define PROFILEBACK          6
#define PROFILELOAD          7
#define CONFIGADVANCE        8
#define PROFILEUP            9
#define PROFILEDOWN          10
#define PROFILEPASS          40

#define NUM_CYCLES           20
#define ZDIP                 21
#define VIBLVL               28
#define PASSEN               29
#define VIBDURATION          30
#define SKIP_COLUMNS         31
#define SKIP_ROWS            32
#define SKIP_SINGLE_POS      33
#define ADVPROF_BACK         34
#define ADVPROF_SAVE         35
#define STAGGERED_TOGGLE     38
#define KEY_CONFIG_HOME      6

#define KEY_CONFIG_LOAD      11
#define KEY_CONFIG_SAVE      12
#define KEY_CONFIG_PROFILE_NAME   13
#define KEY_CONFIG_TUBES_X   14
#define KEY_CONFIG_TUBES_Y   15
#define KEY_CONFIG_PITCH_X   16
#define KEY_CONFIG_PITCH_Y   17
#define KEY_CONFIG_ORIGIN_X  18
#define KEY_CONFIG_ORIGIN_Y  19
#define KEY_CONFIG_PREVIEW   36
#define KEY_CONFIG_PREVIEW_BACK   37

// Movement control tags
#define TAG_MOVE_UP         50
#define TAG_MOVE_DOWN       51
#define TAG_MOVE_LEFT       52
#define TAG_MOVE_RIGHT      53
#define TAG_Z_UP            54
#define TAG_Z_DOWN          55
#define TAG_MOVE_BACK       56

// Dispense test control tags
#define TAG_DISPENSE            60
#define TAG_VIB_U0              61
#define TAG_VIB_U1              62
#define TAG_VIB_U2              63
#define TAG_VIB_U3              64
#define TAG_VIB_U4              65
#define TAG_VIB_TIME_1          66
#define TAG_VIB_TIME_2          67
#define TAG_VIB_TIME_3          68
#define TAG_VIB_TIME_4          69
#define TAG_VIB_TIME_5          70
#define TAG_DISPENSE_BACK       71

// Preview screen control tags
#define TAG_PREVIEW_SIMULATE    80
#define TAG_PREVIEW_STOP        81

#define CONTROLLER_HOME 1
#define CONTROLLER_RUN 2
#define CONTROLLER_MOVE_TEST 3
#define CONTROLLER_DISPENSE_TEST 4
#define CONTROLLER_CONFIG 5

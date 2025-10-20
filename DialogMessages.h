#pragma once

#include <avr/pgmspace.h>

// =============================================================================
// DIALOG TITLES AND SUBTITLES
// =============================================================================

// Note that the following only defines the strings used in the dialogs.
// To edit the dialog behaviour, refer to src/views/common/Dialogs.cpp.

const char DIALOG_TITLE_IR_SENSOR[] PROGMEM = "IR Sensor Error (E01)";
const char DIALOG_SUBTITLE_IR_SENSOR[] PROGMEM = "Could not detect the dispenser head. Please restart the device.";

const char DIALOG_TITLE_MARKER[] PROGMEM = "IR Marker Error (E02)";
const char DIALOG_SUBTITLE_MARKER[] PROGMEM = "Could not detect the dispenser head. Please restart the device.";

const char DIALOG_TITLE_LIMIT_SWITCH[] PROGMEM = "Limit Switch Triggered (E13)";
const char DIALOG_SUBTITLE_LIMIT_SWITCH[] PROGMEM = "Possible movement boundary collision. Please check the head position and resume.";

const char DIALOG_SUBTITLE_LIMIT_SWITCH_HOMING[] PROGMEM = "Possible movement boundary collision. Please check the head position.";

const char DIALOG_TITLE_DISPENSER_ERROR[] PROGMEM = "Dispenser Error (E11)";
const char DIALOG_SUBTITLE_DISPENSER_ERROR[] PROGMEM = "Could not detect the dispenser head. Please restart the device.";

const char DIALOG_TITLE_TIMEOUT[] PROGMEM = "Dispenser Timeout Error (E12)";
const char DIALOG_SUBTITLE_TIMEOUT[] PROGMEM = "Did not receive response from the dispenser head. Please restart the device.";

const char DIALOG_TITLE_PROFILE_SAVED[] PROGMEM = "Profile Saved";
const char DIALOG_SUBTITLE_PROFILE_SAVED[] PROGMEM = "Profile has been saved successfully.";

const char DIALOG_TITLE_PROFILE_LOADED[] PROGMEM = "Profile Loaded";
const char DIALOG_SUBTITLE_PROFILE_LOADED[] PROGMEM = "Profile has been loaded successfully!";

const char DIALOG_TITLE_PASSWORD_MISMATCH[] PROGMEM = "Password Mismatch";
const char DIALOG_SUBTITLE_PASSWORD_MISMATCH[] PROGMEM = "Different passwords were entered, please try again.";

const char DIALOG_TITLE_PASSWORD_CHANGED[] PROGMEM = "Password Changed";
const char DIALOG_SUBTITLE_PASSWORD_CHANGED[] PROGMEM = "Password has been changed successfully!";

const char DIALOG_TITLE_WRONG_PASSWORD[] PROGMEM = "Incorrect Password";
const char DIALOG_SUBTITLE_WRONG_PASSWORD[] PROGMEM = "The password you entered is incorrect. Please try again.";

const char DIALOG_TITLE_PARAMETER_ERROR[] PROGMEM = "Parameter Error (E21)";
const char DIALOG_SUBTITLE_PARAMETER_ERROR[] PROGMEM = "One or more parameters are outside the valid range. Please check the highlighted values.";

const char DIALOG_TITLE_DIMENSION_ERROR[] PROGMEM = "Dimension Error (E22)";
const char DIALOG_SUBTITLE_DIMENSION_ERROR[] PROGMEM = "The parameters exceed the tray boundaries. Please ensure the values are correct.";

const char DIALOG_TITLE_SKIP_VALUES[] PROGMEM = "Skip Values Error (E23)";
const char DIALOG_SUBTITLE_SKIP_VALUES[] PROGMEM = "Skip positions are invalid for current tray configuration. Please update in Advanced settings.";

const char DIALOG_TITLE_SKIP_COUNT[] PROGMEM = "Too Many Skip Positions (E24)";
const char DIALOG_SUBTITLE_SKIP_COUNT[] PROGMEM = "The total number of skip positions exceeds the maximum allowed (%d). Please reduce the number of skips.";
// The %d is replaced with the maximum number of skip positions allowed.

const char DIALOG_TITLE_UNKNOWN[] PROGMEM = "Unknown Error (E99)";
const char DIALOG_SUBTITLE_UNKNOWN[] PROGMEM = "An unknown error occurred.";

const char DIALOG_BTN_RESUME[] PROGMEM = "Resume";
const char DIALOG_BTN_STOP[] PROGMEM = "Stop";
const char DIALOG_BTN_CONTINUE[] PROGMEM = "Continue";

#include "Dialogs.h"

// Dialog strings stored in PROGMEM to save RAM
const char DIALOG_TITLE_IR_SENSOR[] PROGMEM = "IR Sensor Error";
const char DIALOG_SUBTITLE_IR_SENSOR[] PROGMEM = "Could not detect the dispenser head. Please restart the device.";

const char DIALOG_TITLE_MARKER[] PROGMEM = "IR Marker Error";
const char DIALOG_SUBTITLE_MARKER[] PROGMEM = "Could not detect the dispenser head. Please restart the device.";

const char DIALOG_TITLE_LIMIT_SWITCH[] PROGMEM = "Limit Switch Triggered";
const char DIALOG_SUBTITLE_LIMIT_SWITCH[] PROGMEM = "Possible movement boundary collision. Please check the head position and resume.";

const char DIALOG_SUBTITLE_LIMIT_SWITCH_HOMING[] PROGMEM = "Possible movement boundary collision. Please check the head position.";

const char DIALOG_TITLE_DISPENSER_ERROR[] PROGMEM = "Dispenser Error";
const char DIALOG_SUBTITLE_DISPENSER_ERROR[] PROGMEM = "Could not detect the dispenser head. Please restart the device.";

const char DIALOG_TITLE_TIMEOUT[] PROGMEM = "Dispenser Timeout Error";
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

const char DIALOG_TITLE_DIMENSION_ERROR[] PROGMEM = "Dimension Error";
const char DIALOG_SUBTITLE_DIMENSION_ERROR[] PROGMEM = "The parameters exceed the tray boundaries. Please ensure the values are correct.";

const char DIALOG_TITLE_PARAMETER_ERROR[] PROGMEM = "Parameter Error";
const char DIALOG_SUBTITLE_PARAMETER_ERROR[] PROGMEM = "One or more parameters are outside the valid range. Please check the highlighted values.";

const char DIALOG_TITLE_SKIP_VALUES[] PROGMEM = "Skip Values Error";
const char DIALOG_SUBTITLE_SKIP_VALUES[] PROGMEM = "Skip positions are invalid for current tray configuration. Please update in Advanced settings.";

const char DIALOG_TITLE_SKIP_COUNT[] PROGMEM = "Too Many Skip Positions";
const char DIALOG_SUBTITLE_SKIP_COUNT[] PROGMEM = "The total number of skip positions exceeds the maximum allowed (%d). Please reduce the number of skips.";

const char DIALOG_TITLE_UNKNOWN[] PROGMEM = "Unknown Error";
const char DIALOG_SUBTITLE_UNKNOWN[] PROGMEM = "An unknown error occurred.";

const char DIALOG_BTN_RESUME[] PROGMEM = "Resume";
const char DIALOG_BTN_STOP[] PROGMEM = "Stop";
const char DIALOG_BTN_CONTINUE[] PROGMEM = "Continue";

// Displays a dialog with a black box, white outline, title, subtitle, and optional buttons.
// @param phost GPU context.
// @param params DialogParams structure containing title, subtitle, button labels and tags.
void drawBaseDialog(Gpu_Hal_Context_t* phost, const DialogParams& params) {
  // Don't clear the screen - draw on top of existing content

  // Draw an invisible rectangle covering the entire screen to block touches
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(255));    // Use tag 255 for the full-screen overlay
  App_WrCoCmd_Buffer(phost, COLOR_A(0));  // Make it fully transparent
  App_WrCoCmd_Buffer(phost, BEGIN(RECTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(0, 0));        // Top-left corner (0, 0)
  App_WrCoCmd_Buffer(phost, VERTEX2F(5120, 3840));  // Bottom-right corner (320, 240) in 1/16 pixel units
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_A(255));  // Restore full opacity
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Determine button rendering based on what's provided
  bool has_left_btn = (params.left_btn != nullptr && params.left_btn[0] != '\0');
  bool has_right_btn = (params.right_btn != nullptr && params.right_btn[0] != '\0');
  bool has_buttons = has_left_btn || has_right_btn;

  // Calculate dialog dimensions
  const int box_width = 260;
  const int box_x = 30;  // (320 - 260) / 2
  const int button_height = 30;
  const int button_spacing = 10;

  // Calculate box height based on whether buttons are present
  int box_height;
  if (has_buttons) {
    box_height = 155;  // Original height with buttons
  } else {
    box_height = 110;  // Reduced height without buttons (155 - 30 - 15 spacing)
  }

  // Center the dialog vertically on screen (screen height = 240)
  int box_y = (240 - box_height) / 2;

  // Draw black box with white outline in the middle of the screen (slightly transparent)
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, COLOR_A(220));  // Slightly transparent (220/255 opacity)
  App_WrCoCmd_Buffer(phost, BEGIN(RECTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(box_x * 16, box_y * 16));                               // Top-left corner
  App_WrCoCmd_Buffer(phost, VERTEX2F((box_x + box_width) * 16, (box_y + box_height) * 16));  // Bottom-right corner
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_A(255));  // Restore full opacity

  // Draw white outline
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, LINE_WIDTH(16));
  App_WrCoCmd_Buffer(phost, BEGIN(LINE_STRIP));
  App_WrCoCmd_Buffer(phost, VERTEX2F(box_x * 16, box_y * 16));                               // Top-left
  App_WrCoCmd_Buffer(phost, VERTEX2F((box_x + box_width) * 16, box_y * 16));                 // Top-right
  App_WrCoCmd_Buffer(phost, VERTEX2F((box_x + box_width) * 16, (box_y + box_height) * 16));  // Bottom-right
  App_WrCoCmd_Buffer(phost, VERTEX2F(box_x * 16, (box_y + box_height) * 16));                // Bottom-left
  App_WrCoCmd_Buffer(phost, VERTEX2F(box_x * 16, box_y * 16));                               // Close the box
  App_WrCoCmd_Buffer(phost, END());

  // Display title text (relative to box position)
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Text(phost, 160, box_y + 23, 28, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, params.title);

  // Display subtitle text with wrapping (relative to box position)
  Gpu_CoCmd_FillWidth(phost, 240);  // Set wrap width to 240 pixels (box is 260 wide, leave 10px margin each side)
  Gpu_CoCmd_Text(phost, 160, box_y + 43, 21, OPT_CENTERX | OPT_FILL, params.subtitle);
  Gpu_CoCmd_FillWidth(phost, 0);  // Reset fill width

  // Render buttons if present
  if (has_buttons) {
    int button_y = box_y + box_height - button_height - button_spacing;

    if (has_left_btn && has_right_btn) {
      // Both buttons - each takes half width with padding
      App_WrCoCmd_Buffer(phost, TAG_MASK(1));
      App_WrCoCmd_Buffer(phost, TAG(params.left_tag));
      Gpu_CoCmd_FgColor(phost, 0x00A2E8);
      Gpu_CoCmd_Button(phost, box_x + 10, button_y, 115, button_height, 21, 0, params.left_btn);

      App_WrCoCmd_Buffer(phost, TAG(params.right_tag));
      Gpu_CoCmd_Button(phost, box_x + 135, button_y, 115, button_height, 21, 0, params.right_btn);
      App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    } else if (has_left_btn) {
      // Only left button - full width with padding
      App_WrCoCmd_Buffer(phost, TAG_MASK(1));
      App_WrCoCmd_Buffer(phost, TAG(params.left_tag));
      Gpu_CoCmd_FgColor(phost, 0x00A2E8);
      Gpu_CoCmd_Button(phost, box_x + 10, button_y, 240, button_height, 21, 0, params.left_btn);
      App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    } else if (has_right_btn) {
      // Only right button - full width with padding
      App_WrCoCmd_Buffer(phost, TAG_MASK(1));
      App_WrCoCmd_Buffer(phost, TAG(params.right_tag));
      Gpu_CoCmd_FgColor(phost, 0x00A2E8);
      Gpu_CoCmd_Button(phost, box_x + 10, button_y, 240, button_height, 21, 0, params.right_btn);
      App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    }
  }
}

// Displays a dialog based on dialog code.
// @param phost GPU context.
// @param dialog_code The dialog code to display.
void drawDialog(Gpu_Hal_Context_t* phost, int dialog_code) {
  // Temporary RAM buffers to copy PROGMEM strings into
  static char title_buffer[30];
  static char subtitle_buffer[120];
  static char left_btn_buffer[15];
  static char right_btn_buffer[15];
  
  DialogParams params;

  switch (dialog_code) {
    case DIALOG_ERROR_IR_SENSOR:
      params.title = DIALOG_TITLE_IR_SENSOR;
      params.subtitle = DIALOG_SUBTITLE_IR_SENSOR;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_MARKER_NOT_DETECTED:
      params.title = DIALOG_TITLE_MARKER;
      params.subtitle = DIALOG_SUBTITLE_MARKER;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_LIMIT_SWITCH:
      params.title = DIALOG_TITLE_LIMIT_SWITCH;
      params.subtitle = DIALOG_SUBTITLE_LIMIT_SWITCH;
      params.left_btn = DIALOG_BTN_RESUME;
      params.right_btn = DIALOG_BTN_STOP;
      params.left_tag = TAG_START;
      params.right_tag = TAG_STOP;
      break;

    case DIALOG_ERROR_LIMIT_SWITCH_HOMING:
      params.title = DIALOG_TITLE_LIMIT_SWITCH;
      params.subtitle = DIALOG_SUBTITLE_LIMIT_SWITCH_HOMING;
      params.left_btn = DIALOG_BTN_CONTINUE;
      params.right_btn = nullptr;
      params.left_tag = TAG_CONTINUE;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_ACK_ERROR:
      params.title = DIALOG_TITLE_DISPENSER_ERROR;
      params.subtitle = DIALOG_SUBTITLE_DISPENSER_ERROR;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_CYCLE_TIMEOUT:
      params.title = DIALOG_TITLE_TIMEOUT;
      params.subtitle = DIALOG_SUBTITLE_TIMEOUT;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_PROFILE_SAVED:
      params.title = DIALOG_TITLE_PROFILE_SAVED;
      params.subtitle = DIALOG_SUBTITLE_PROFILE_SAVED;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_PROFILE_LOADED:
      params.title = DIALOG_TITLE_PROFILE_LOADED;
      params.subtitle = DIALOG_SUBTITLE_PROFILE_LOADED;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_PASSWORD_MISMATCH:
      params.title = DIALOG_TITLE_PASSWORD_MISMATCH;
      params.subtitle = DIALOG_SUBTITLE_PASSWORD_MISMATCH;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_PASSWORD_CHANGED:
      params.title = DIALOG_TITLE_PASSWORD_CHANGED;
      params.subtitle = DIALOG_SUBTITLE_PASSWORD_CHANGED;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_WRONG_PASSWORD:
      params.title = DIALOG_TITLE_WRONG_PASSWORD;
      params.subtitle = DIALOG_SUBTITLE_WRONG_PASSWORD;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_DIMENSION:
      params.title = DIALOG_TITLE_DIMENSION_ERROR;
      params.subtitle = DIALOG_SUBTITLE_DIMENSION_ERROR;
      params.left_btn = DIALOG_BTN_CONTINUE;
      params.right_btn = nullptr;
      params.left_tag = TAG_CONTINUE;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_PARAMETER:
      params.title = DIALOG_TITLE_PARAMETER_ERROR;
      params.subtitle = DIALOG_SUBTITLE_PARAMETER_ERROR;
      params.left_btn = DIALOG_BTN_CONTINUE;
      params.right_btn = nullptr;
      params.left_tag = TAG_CONTINUE;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_SKIP_VALUES:
      params.title = DIALOG_TITLE_SKIP_VALUES;
      params.subtitle = DIALOG_SUBTITLE_SKIP_VALUES;
      params.left_btn = DIALOG_BTN_CONTINUE;
      params.right_btn = nullptr;
      params.left_tag = TAG_CONTINUE;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_SKIP_COUNT_EXCEEDED:
      params.title = DIALOG_TITLE_SKIP_COUNT;
      // Format the subtitle with the actual MAX_SKIP_POSITIONS value
      strcpy_P(subtitle_buffer, params.subtitle);
      sprintf(subtitle_buffer, subtitle_buffer, MAX_SKIP_POSITIONS);
      params.subtitle = subtitle_buffer;
      params.left_btn = DIALOG_BTN_CONTINUE;
      params.right_btn = nullptr;
      params.left_tag = TAG_CONTINUE;
      params.right_tag = 0;
      break;

    default:
      params.title = DIALOG_TITLE_UNKNOWN;
      params.subtitle = DIALOG_SUBTITLE_UNKNOWN;
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;
  }

  // Copy PROGMEM strings to RAM buffers for GPU rendering
  if (params.title != nullptr) {
    strcpy_P(title_buffer, params.title);
    params.title = title_buffer;
  }
  
  if (params.subtitle != nullptr) {
    strcpy_P(subtitle_buffer, params.subtitle);
    params.subtitle = subtitle_buffer;
  }
  
  if (params.left_btn != nullptr) {
    strcpy_P(left_btn_buffer, params.left_btn);
    params.left_btn = left_btn_buffer;
  }
  
  if (params.right_btn != nullptr) {
    strcpy_P(right_btn_buffer, params.right_btn);
    params.right_btn = right_btn_buffer;
  }

  drawBaseDialog(phost, params);
}

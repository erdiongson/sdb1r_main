#include "Dialogs.h"

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
  DialogParams params;

  switch (dialog_code) {
    case DIALOG_ERROR_IR_SENSOR:
      params.title = "IR Sensor Error";
      params.subtitle = "Could not detect the dispenser head. Please restart the device.";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_MARKER_NOT_DETECTED:
      params.title = "IR Marker Error";
      params.subtitle = "Could not detect the dispenser head. Please restart the device.";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_LIMIT_SWITCH:
      params.title = "Limit Switch Triggered";
      params.subtitle = "Possible movement boundary collision. Please check the head position and resume.";
      params.left_btn = "Resume";
      params.right_btn = "Stop";
      params.left_tag = TAG_START;
      params.right_tag = TAG_STOP;
      break;

    case DIALOG_ERROR_LIMIT_SWITCH_HOMING:
      params.title = "Limit Switch Triggered";
      params.subtitle = "Possible movement boundary collision. Please check the head position.";
      params.left_btn = "Continue";
      params.right_btn = nullptr;
      params.left_tag = TAG_CONTINUE;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_ACK_ERROR:
      params.title = "Dispenser Error";
      params.subtitle = "Could not detect the dispenser head. Please restart the device.";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_CYCLE_TIMEOUT:
      params.title = "Dispenser Timeout Error";
      params.subtitle = "Did not receive response from the dispenser head. Please restart the device.";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_PROFILE_SAVED:
      params.title = "Profile Saved";
      params.subtitle = "Profile has been saved successfully.";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_PROFILE_LOADED:
      params.title = "Profile Loaded";
      params.subtitle = "Profile has been loaded successfully!";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_PASSWORD_MISMATCH:
      params.title = "Password Mismatch";
      params.subtitle = "Different passwords were entered, please try again.";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_PASSWORD_CHANGED:
      params.title = "Password Changed";
      params.subtitle = "Password has been changed successfully!";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_WRONG_PASSWORD:
      params.title = "Incorrect Password";
      params.subtitle = "The password you entered is incorrect. Please try again.";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_DIMENSION:
      params.title = "Dimension Error";
      params.subtitle = "The parameters exceed the tray boundaries. Please ensure the values are correct.";
      params.left_btn = "Continue";
      params.right_btn = nullptr;
      params.left_tag = TAG_CONTINUE;
      params.right_tag = 0;
      break;

    case DIALOG_ERROR_PARAMETER:
      params.title = "Parameter Error";
      params.subtitle = "One or more parameters are outside the valid range. Please check the highlighted values.";
      params.left_btn = "Continue";
      params.right_btn = nullptr;
      params.left_tag = TAG_CONTINUE;
      params.right_tag = 0;
      break;

    default:
      params.title = "Unknown Error";
      params.subtitle = "An unknown error occurred.";
      params.left_btn = nullptr;
      params.right_btn = nullptr;
      params.left_tag = 0;
      params.right_tag = 0;
      break;
  }

  drawBaseDialog(phost, params);
}

#include "ErrorMessages.h"

// Displays an error/message dialog with a black box, white outline, title, subtitle, and optional buttons.
// @param phost GPU context.
// @param params ErrorMessageParams structure containing title, subtitle, button labels and tags.
void draw_base_error_message(Gpu_Hal_Context_t *phost, const ErrorMessageParams& params) {
  // Don't clear the screen - draw on top of existing content

  // Draw an invisible rectangle covering the entire screen to block touches
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(255));  // Use tag 255 for the full-screen overlay
  App_WrCoCmd_Buffer(phost, COLOR_A(0));  // Make it fully transparent
  App_WrCoCmd_Buffer(phost, BEGIN(RECTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(0, 0));        // Top-left corner (0, 0)
  App_WrCoCmd_Buffer(phost, VERTEX2F(5120, 3840));  // Bottom-right corner (320, 240) in 1/16 pixel units
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_A(255));  // Restore full opacity
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  
  // Draw black box with white outline in the middle of the screen (slightly transparent)
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, COLOR_A(220));  // Slightly transparent (220/255 opacity)
  App_WrCoCmd_Buffer(phost, BEGIN(RECTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(640, 640));   // Top-left corner (40, 40) - larger box
  App_WrCoCmd_Buffer(phost, VERTEX2F(4480, 2880)); // Bottom-right corner (280, 180) - larger box
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_A(255));  // Restore full opacity
  
  // Draw white outline
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, LINE_WIDTH(16));
  App_WrCoCmd_Buffer(phost, BEGIN(LINE_STRIP));
  App_WrCoCmd_Buffer(phost, VERTEX2F(640, 640));   // Top-left
  App_WrCoCmd_Buffer(phost, VERTEX2F(4480, 640));  // Top-right
  App_WrCoCmd_Buffer(phost, VERTEX2F(4480, 2880)); // Bottom-right
  App_WrCoCmd_Buffer(phost, VERTEX2F(640, 2880));  // Bottom-left
  App_WrCoCmd_Buffer(phost, VERTEX2F(640, 640));   // Close the box
  App_WrCoCmd_Buffer(phost, END());
  
  // Display title text
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Text(phost, 155, 70, 28, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, params.title);
  
  // Display subtitle text with wrapping
  Gpu_CoCmd_FillWidth(phost, 220);  // Set wrap width to 220 pixels (box is 240 wide, leave 10px margin each side)
  Gpu_CoCmd_Text(phost, 160, 100, 18, OPT_CENTERX | OPT_FILL, params.subtitle);
  Gpu_CoCmd_FillWidth(phost, 0);  // Reset fill width
  
  // Determine button rendering based on what's provided
  bool hasLeftBtn = (params.leftBtn != nullptr && params.leftBtn[0] != '\0');
  bool hasRightBtn = (params.rightBtn != nullptr && params.rightBtn[0] != '\0');
  
  if (hasLeftBtn && hasRightBtn) {
    // Both buttons - render on left and right
    App_WrCoCmd_Buffer(phost, TAG_MASK(1));
    App_WrCoCmd_Buffer(phost, TAG(params.leftTag));
    Gpu_CoCmd_FgColor(phost, 0x00A2E8);
    Gpu_CoCmd_Button(phost, 70, 130, 60, 30, 21, 0, params.leftBtn);
    
    App_WrCoCmd_Buffer(phost, TAG(params.rightTag));
    Gpu_CoCmd_Button(phost, 180, 130, 60, 30, 21, 0, params.rightBtn);
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  } else if (hasLeftBtn) {
    // Only left button - render in center
    App_WrCoCmd_Buffer(phost, TAG_MASK(1));
    App_WrCoCmd_Buffer(phost, TAG(params.leftTag));
    Gpu_CoCmd_FgColor(phost, 0x00A2E8);
    Gpu_CoCmd_Button(phost, 125, 130, 60, 30, 21, 0, params.leftBtn);
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  } else if (hasRightBtn) {
    // Only right button - render in center
    App_WrCoCmd_Buffer(phost, TAG_MASK(1));
    App_WrCoCmd_Buffer(phost, TAG(params.rightTag));
    Gpu_CoCmd_FgColor(phost, 0x00A2E8);
    Gpu_CoCmd_Button(phost, 125, 130, 60, 30, 21, 0, params.rightBtn);
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  }
  // If neither button is provided, don't render any buttons
}

// Displays an error message based on error code.
// @param phost GPU context.
// @param error_code The error code to display.
void draw_error_message(Gpu_Hal_Context_t *phost, int error_code) {
  ErrorMessageParams params;
  
  switch (error_code) {
    case ERROR_TEST:
      params.title = "Test Error";
      params.subtitle = "This is a test error message";
      params.leftBtn = "Cancel";
      params.rightBtn = "OK";
      params.leftTag = 100;
      params.rightTag = 101;
      break;
      
    case ERROR_IR_SENSOR:
      params.title = "IR Sensor Error";
      params.subtitle = "IR Sensor Detection Failed. Please Restart.";
      params.leftBtn = nullptr;
      params.rightBtn = "OK";
      params.leftTag = 0;
      params.rightTag = 102;
      break;
      
    case ERROR_MARKER_NOT_DETECTED:
      params.title = "Dispenser Head Stuck";
      params.subtitle = "Please check the head and restart.";
      params.leftBtn = nullptr;
      params.rightBtn = "OK";
      params.leftTag = 0;
      params.rightTag = 103;
      break;
      
    case ERROR_LIMIT_SWITCH:
      params.title = "Limit Switch Error";
      params.subtitle = "Limit switch triggered unexpectedly. Please check the head and restart.";
      params.leftBtn = nullptr;
      params.rightBtn = "OK";
      params.leftTag = 0;
      params.rightTag = 104;
      break;
      
    case ERROR_ACK_ERROR:
      params.title = "Dispenser Error";
      params.subtitle = "Please restart the device.";
      params.leftBtn = nullptr;
      params.rightBtn = "OK";
      params.leftTag = 0;
      params.rightTag = 103;
      break;
      
    default:
      params.title = "Unknown Error";
      params.subtitle = "An unknown error occurred.";
      params.leftBtn = nullptr;
      params.rightBtn = "OK";
      params.leftTag = 0;
      params.rightTag = 105;
      break;
  }
  
  draw_base_error_message(phost, params);
}

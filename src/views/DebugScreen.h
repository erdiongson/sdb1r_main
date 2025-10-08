#ifndef DEBUG_SCREEN_H
#define DEBUG_SCREEN_H

#include "../gpu/App_Common.h"
#include "../Constants.h"

// Parameters for Debug Screen display.
struct DebugScreenParams {
  uint8_t currentSize;  // 0 = small, 1 = large
  bool blankedEeprom;   // true = show (Done) on Blank EEPROM button
  bool profilesReset;   // true = show (Done) on Reset Profiles button
};

// Draws the debug screen with test and utility buttons.
// @param phost Pointer to the GPU HAL context.
// @param params Debug screen parameters.
inline void draw_debug_screen(Gpu_Hal_Context_t *phost, DebugScreenParams params) {
  char buf[100];

  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

  // Title
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 165, 0));
  Gpu_CoCmd_Text(phost, 159, 20, 30, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "DEBUG MENU");
  
  // Divider line
  App_WrCoCmd_Buffer(phost, COLOR_RGB(193, 64, 0));
  App_WrCoCmd_Buffer(phost, BEGIN(LINES));
  App_WrCoCmd_Buffer(phost, VERTEX2F(0, 720));
  App_WrCoCmd_Buffer(phost, VERTEX2F(5120, 720));
  App_WrCoCmd_Buffer(phost, END());
  
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));

  // Button layout
  int button_y = 50;
  int button_spacing = 30;
  int button_height = 28;

  // Move Test and Dispenser Test buttons side-by-side (50% width each)
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_MOVE_TEST));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  Gpu_CoCmd_Button(phost, 25, button_y, 130, button_height, 26, 0, "Move Test");
  
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_DISPENSER_TEST));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  Gpu_CoCmd_Button(phost, 165, button_y, 130, button_height, 26, 0, "Dispenser Test");
  button_y += button_spacing;

  // Leave empty space where second row would be
  button_y += button_spacing;

  // Blank EEPROM button
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_BLANK_EEPROM));
  Gpu_CoCmd_FgColor(phost, 0xFF6600);
  if (params.blankedEeprom) {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, "Blank EEPROM (Done)");
  } else {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, "Blank EEPROM");
  }
  button_y += button_spacing;

  // Reset Profiles button
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_RESET_PROFILES));
  Gpu_CoCmd_FgColor(phost, 0xFF6600);
  if (params.profilesReset) {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, "Reset Profiles (Done)");
  } else {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, "Reset Profiles");
  }
  button_y += button_spacing;

  // Toggle Size button
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_TOGGLE_SIZE));
  Gpu_CoCmd_FgColor(phost, 0x9370DB);
  sprintf(buf, "Toggle Size (Current: %s)", params.currentSize == 1 ? "L" : "S");
  Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, buf);

  // Back button at bottom left (smaller, similar to PreviewScreen)
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_BACK));
  Gpu_CoCmd_FgColor(phost, 0x808080);
  Gpu_CoCmd_Button(phost, 10, 210, 62, 26, 21, 0, "Back");

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  Disp_End(phost);
}

#endif

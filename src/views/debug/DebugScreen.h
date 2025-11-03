#pragma once

#include "../../gpu/App_Common.h"
#include "../../Constants.h"
#include "../common/Dialogs.h"
#include "../ViewCommon.h"

// Parameters for Debug Screen display.
struct DebugScreenParams {
  bool blanked_eeprom;         // true = show (Done) on Blank EEPROM button
  bool profiles_reset;         // true = show (Done) on Reset Profiles button
  bool debug_profiles_loaded;  // true = show (Done) on Use Debug Profiles button
  uint8_t dialog_code;         // Dialog codes are < 50
};

// Draws the debug screen with test and utility buttons.
// @param phost Pointer to the GPU HAL context.
// @param params Debug screen parameters.
inline void drawDebugScreen(Gpu_Hal_Context_t* phost, DebugScreenParams params) {

  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

  // Title
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 165, 0));
  Gpu_CoCmd_Text(phost, 159, 15, 28, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("DEBUG MENU")));

  // Divider line
  App_WrCoCmd_Buffer(phost, COLOR_RGB(193, 64, 0));
  App_WrCoCmd_Buffer(phost, BEGIN(LINES));
  App_WrCoCmd_Buffer(phost, VERTEX2F(0, 35 * 16));
  App_WrCoCmd_Buffer(phost, VERTEX2F(DispWidth * 16, 35 * 16));
  App_WrCoCmd_Buffer(phost, END());

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));

  // Button layout
  int button_y = 45;
  int button_spacing = 30;
  int button_height = 28;

  // Move Test and Stepper Test buttons side-by-side (50% width each)
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_MOVE_TEST));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  Gpu_CoCmd_Button(phost, 25, button_y, 130, button_height, 26, 0, PROGMEM_STR(F("Move Test")));

  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_STEPPER_TEST));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  Gpu_CoCmd_Button(phost, 165, button_y, 130, button_height, 26, 0, PROGMEM_STR(F("Stepper Test")));
  button_y += button_spacing;

  // Dispenser Test and Dialog Test buttons side-by-side (50% width each)
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_DISPENSER_TEST));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  Gpu_CoCmd_Button(phost, 25, button_y, 130, button_height, 26, 0, PROGMEM_STR(F("Dispenser Test")));

  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_DIALOG_TEST));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  Gpu_CoCmd_Button(phost, 165, button_y, 130, button_height, 26, 0, PROGMEM_STR(F("Dialog Test")));
  button_y += button_spacing;

  // Add spacing before utility buttons
  button_y += 10;

  // Blank EEPROM button
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_BLANK_EEPROM));
  Gpu_CoCmd_FgColor(phost, 0xFF6600);
  if (params.blanked_eeprom) {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, PROGMEM_STR(F("Blank EEPROM (Done)")));
  } else {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, PROGMEM_STR(F("Blank EEPROM")));
  }
  button_y += button_spacing;

  // Reset Profiles button
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_RESET_PROFILES));
  Gpu_CoCmd_FgColor(phost, 0xFF6600);
  if (params.profiles_reset) {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, PROGMEM_STR(F("Reset Profiles (Done)")));
  } else {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, PROGMEM_STR(F("Reset Profiles")));
  }
  button_y += button_spacing;

  // Use Debug Profiles button
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_USE_DEBUG_PROFILES));
  Gpu_CoCmd_FgColor(phost, 0xFF6600);
  if (params.debug_profiles_loaded) {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, PROGMEM_STR(F("Use Debug Profiles (Done)")));
  } else {
    Gpu_CoCmd_Button(phost, 25, button_y, 270, button_height, 26, 0, PROGMEM_STR(F("Use Debug Profiles")));
  }
  button_y += button_spacing;

  // Back button at bottom left
  App_WrCoCmd_Buffer(phost, TAG(TAG_DEBUG_BACK));
  Gpu_CoCmd_FgColor(phost, 0xAA0000);
  Gpu_CoCmd_Button(phost, 10, 210, 50, 22, 20, 0, PROGMEM_STR(F("Back")));

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Draw dialog if dialog_code is set
  if (params.dialog_code > 0) {
    drawDialog(phost, params.dialog_code);
  }

  Disp_End(phost);
}

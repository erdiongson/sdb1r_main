#pragma once

#include "../../gpu/App_Common.h"
#include "../../Constants.h"
#include "../../../Config.h"
#include "../ViewCommon.h"

// Draws the configuration screen showing compile-time configuration parameters.
// @param phost Pointer to the GPU HAL context.
inline void drawConfigScreen(Gpu_Hal_Context_t* phost) {
  char* temp_buffer = g_view_temp_buffer;  // Use shared buffer

  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));

  // Title
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 165, 0));
  Gpu_CoCmd_Text(phost, 159, 15, 28, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("CURRENT CONFIG")));

  // Divider line
  App_WrCoCmd_Buffer(phost, COLOR_RGB(193, 64, 0));
  App_WrCoCmd_Buffer(phost, BEGIN(LINES));
  App_WrCoCmd_Buffer(phost, VERTEX2F(0, 35 * 16));
  App_WrCoCmd_Buffer(phost, VERTEX2F(DispWidth * 16, 35 * 16));
  App_WrCoCmd_Buffer(phost, END());

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 127));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));

  // Starting Y position for text
  int text_y = 45;
  int line_spacing = 15;
  int font = 21;

  // Display FWVERM
  char fwverm[10];
  strcpy_P(fwverm, FWVERM);
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("FWVERM: %s"), fwverm);
  Gpu_CoCmd_Text(phost, 8, text_y, font, 0, temp_buffer);
  text_y += line_spacing;

  // Display DEBUG_FLAGS (NO_LOG, ONLY_SCREEN, MEMORY_MONITOR)
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("DEBUG_FLAGS: %d, %d, %d"), DEBUG_NO_LOG, DEBUG_ONLY_SCREEN, DEBUG_MEMORY_MONITOR);
  Gpu_CoCmd_Text(phost, 8, text_y, font, 0, temp_buffer);
  text_y += line_spacing;

  // Display Z_DISABLED
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("Z_DISABLED: %d"), Z_DISABLED ? 1 : 0);
  Gpu_CoCmd_Text(phost, 8, text_y, font, 0, temp_buffer);
  text_y += line_spacing;

  // Display PRIME_DISPENSE_NUM
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("PRIME_DISPENSE_NUM: %d"), PRIME_DISPENSE_NUM);
  Gpu_CoCmd_Text(phost, 8, text_y, font, 0, temp_buffer);
  text_y += line_spacing;

  // Add spacing
  text_y += 5;

  // Display TUBES_MAX (X,Y pair)
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("TUBES_MAX: %d, %d"), TUBES_X_MAX, TUBES_Y_MAX);
  Gpu_CoCmd_Text(phost, 8, text_y, font, 0, temp_buffer);
  text_y += line_spacing;

  // Display TRAY_MAX (X,Y pair)
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("TRAY_MAX: %d, %d"), TRAY_X_MAX, TRAY_Y_MAX);
  Gpu_CoCmd_Text(phost, 8, text_y, font, 0, temp_buffer);
  text_y += line_spacing;

  // Display ORIGIN_MIN (X,Y pair)
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("ORIGIN_MIN: %d, %d"), ORIGIN_X_MIN, ORIGIN_Y_MIN);
  Gpu_CoCmd_Text(phost, 8, text_y, font, 0, temp_buffer);
  text_y += line_spacing;

  // Display PITCH_MIN (X,Y pair)
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("PITCH_MIN: %d, %d"), PITCH_X_MIN, PITCH_Y_MIN);
  Gpu_CoCmd_Text(phost, 8, text_y, font, 0, temp_buffer);
  text_y += line_spacing;

  // Display Z_DIP_MAX
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("Z_DIP_MAX: %d"), Z_DIP_MAX);
  Gpu_CoCmd_Text(phost, 8, text_y, font, 0, temp_buffer);
  text_y += line_spacing;

  // Back button at bottom left
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_BACK));
  Gpu_CoCmd_FgColor(phost, 0xAA0000);
  Gpu_CoCmd_Button(phost, 10, 210, 50, 22, 20, 0, PROGMEM_STR(F("Back")));

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  Disp_End(phost);
}

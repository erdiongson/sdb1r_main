/*
 * DispenseTestScreen.h
 *
 * Dispense test screen for SDB-1R
 * Provides controls for testing dispense functionality with vibration settings
 *
 * Author: XentiQ
 * Created: 2024-09-30
 */

#pragma once

#include "../../gpu/App_Common.h"
#include "../../Constants.h"
#include "../common/Dialogs.h"
#include "../ViewCommon.h"

// Dispense test screen parameters.
struct DispenseTestScreenParams {
  const char* status_message;
  int repeat_count;
  int current_repeat_count;
  int total_repeat_count;
  bool is_dispensing;
};

// Display the dispense test screen.
// Creates a test interface with:
// - Row 1: Vibration level selection (U0-U4)
// - Row 2: Vibration time selection (1-5s)
// - Row 3: Repeat button, Dispense/Stop button, Progress text
// - Back button
// @param phost Pointer to GPU HAL context.
// @param params Screen parameters including status message and repeat info.
void drawDispenseTestScreen(Gpu_Hal_Context_t* phost, const DispenseTestScreenParams& params);

// Display the dispense test screen.
// Creates a test interface for dispense functionality testing.
// @param phost Pointer to GPU HAL context.
// @param params Screen parameters including status message.
void drawDispenseTestScreen(Gpu_Hal_Context_t* phost, const DispenseTestScreenParams& params) {
  char* temp_buffer = g_view_temp_buffer;  // Use shared buffer (60 bytes)
  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  // Clear screen
  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);

  // Header section
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Text(phost, DispWidth / 2, 15, 28, OPT_CENTER | OPT_FORMAT, PROGMEM_STR(F("Dispense Test")));

  // Draw separator line
  App_WrCoCmd_Buffer(phost, COLOR_RGB(193, 64, 0));
  App_WrCoCmd_Buffer(phost, BEGIN(LINES));
  App_WrCoCmd_Buffer(phost, VERTEX2F(0, 35 * 16));
  App_WrCoCmd_Buffer(phost, VERTEX2F(DispWidth * 16, 35 * 16));
  App_WrCoCmd_Buffer(phost, END());

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Calculate layout dimensions
  int32_t row_height = 40;
  int32_t start_y = 50;
  int32_t button_width = 45;
  int32_t button_height = 25;
  int32_t button_spacing = 6;

  // Row 1: Vibration Level Selection (U0-U4)
  int32_t vib_level_y = start_y;
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Calculate starting position for centered buttons
  int32_t total_vib_width = (5 * button_width) + (4 * button_spacing);
  int32_t vib_start_x = (DispWidth - total_vib_width) / 2;

  // U0 button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U0));
  Gpu_CoCmd_Button(phost, vib_start_x, vib_level_y, button_width, button_height, 20, 0, PROGMEM_STR(F("U0")));

  // U1 button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U1));
  Gpu_CoCmd_Button(phost, vib_start_x + (button_width + button_spacing), vib_level_y, button_width, button_height, 20,
                   0, PROGMEM_STR(F("U1")));

  // U2 button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U2));
  Gpu_CoCmd_Button(phost, vib_start_x + 2 * (button_width + button_spacing), vib_level_y, button_width, button_height,
                   20, 0, PROGMEM_STR(F("U2")));

  // U3 button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U3));
  Gpu_CoCmd_Button(phost, vib_start_x + 3 * (button_width + button_spacing), vib_level_y, button_width, button_height,
                   20, 0, PROGMEM_STR(F("U3")));

  // U4 button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U4));
  Gpu_CoCmd_Button(phost, vib_start_x + 4 * (button_width + button_spacing), vib_level_y, button_width, button_height,
                   20, 0, PROGMEM_STR(F("U4")));

  // Row 2: Vibration Time Selection (1-5s) - 4px gap below vibration level
  int32_t vib_time_y = vib_level_y + button_height + 4;
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Calculate starting position for centered buttons
  int32_t total_time_width = (5 * button_width) + (4 * button_spacing);
  int32_t time_start_x = (DispWidth - total_time_width) / 2;

  // 1s button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_1));
  Gpu_CoCmd_Button(phost, time_start_x, vib_time_y, button_width, button_height, 20, 0, PROGMEM_STR(F("1s")));

  // 2s button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_2));
  Gpu_CoCmd_Button(phost, time_start_x + (button_width + button_spacing), vib_time_y, button_width, button_height, 20,
                   0, PROGMEM_STR(F("2s")));

  // 3s button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_3));
  Gpu_CoCmd_Button(phost, time_start_x + 2 * (button_width + button_spacing), vib_time_y, button_width, button_height,
                   20, 0, PROGMEM_STR(F("3s")));

  // 4s button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_4));
  Gpu_CoCmd_Button(phost, time_start_x + 3 * (button_width + button_spacing), vib_time_y, button_width, button_height,
                   20, 0, PROGMEM_STR(F("4s")));

  // 5s button
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_5));
  Gpu_CoCmd_Button(phost, time_start_x + 4 * (button_width + button_spacing), vib_time_y, button_width, button_height,
                   20, 0, PROGMEM_STR(F("5s")));

  // Horizontal divider line
  int32_t divider_y = vib_time_y + button_height + 8;
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(128, 128, 128));
  App_WrCoCmd_Buffer(phost, BEGIN(LINES));
  App_WrCoCmd_Buffer(phost, VERTEX2F(20 * 16, divider_y * 16));
  App_WrCoCmd_Buffer(phost, VERTEX2F((DispWidth - 20) * 16, divider_y * 16));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Row 3: Repeat button, Dispense/Stop button, Progress text - below divider line
  int32_t row3_y = divider_y + 8;
  int32_t row3_button_height = button_height;  // Same height as other buttons for consistency
  int32_t row3_spacing = 6;  // Same spacing as other rows

  // Repeat button (left-aligned with first two rows)
  int32_t repeat_width = 60;
  int32_t row3_start_x = vib_start_x;  // Align with vibration buttons
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("Repeat: %d"), params.repeat_count);
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_DISPENSE_REPEAT));
  Gpu_CoCmd_Button(phost, row3_start_x, row3_y, repeat_width, row3_button_height, 20, 0, temp_buffer);

  // Dispense/Stop button (right next to repeat button)
  int32_t dispense_width = 70;
  int32_t dispense_x = row3_start_x + repeat_width + row3_spacing;
  
  if (params.is_dispensing) {
    Gpu_CoCmd_FgColor(phost, 0xFF0000);  // Red for stop
    App_WrCoCmd_Buffer(phost, TAG(TAG_DISPENSE_STOP));
    Gpu_CoCmd_Button(phost, dispense_x, row3_y, dispense_width, row3_button_height, 20, 0, PROGMEM_STR(F("STOP")));
  } else {
    Gpu_CoCmd_FgColor(phost, 0xFF4500);  // Orange red for dispense
    App_WrCoCmd_Buffer(phost, TAG(TAG_DISPENSE));
    Gpu_CoCmd_Button(phost, dispense_x, row3_y, dispense_width, row3_button_height, 20, 0, PROGMEM_STR(F("DISPENSE")));
  }

  // Status message with progress (right of dispense button)
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(200, 200, 200));
  int32_t text_x = dispense_x + dispense_width + 10;
  
  // Combine status message with progress count if active
  if (params.total_repeat_count > 0) {
    snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("%s (%d/%d)"), params.status_message, params.current_repeat_count, params.total_repeat_count);
  } else {
    snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("%s"), params.status_message);
  }
  
  Gpu_CoCmd_Text(phost, text_x, row3_y + row3_button_height / 2, 20, OPT_CENTERY, temp_buffer);
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Row 4: Get Firmware Version button - below row 3
  int32_t row4_y = row3_y + row3_button_height + 6;
  int32_t fw_button_width = 100;
  Gpu_CoCmd_FgColor(phost, 0x0066CC);  // Blue color for firmware button
  App_WrCoCmd_Buffer(phost, TAG(TAG_GET_FW_VERSION));
  Gpu_CoCmd_Button(phost, row3_start_x, row4_y, fw_button_width, row3_button_height, 20, 0, PROGMEM_STR(F("Get FW Ver")));

  // Back button (top left, vertically aligned with title)
  Gpu_CoCmd_FgColor(phost, 0xAA0000);
  App_WrCoCmd_Buffer(phost, TAG(TAG_DISPENSE_BACK));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 10, 4, 50, 22, 20, 0, PROGMEM_STR(F("Back")));

  // Finalize display
  Disp_End(phost);
}

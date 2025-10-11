/*
 * DispenseTestScreen.h
 * 
 * Dispense test screen for SDB-1R
 * Provides controls for testing dispense functionality with vibration settings
 * 
 * Author: XentiQ
 * Created: 2024-09-30
 */

#ifndef _DISPENSE_TEST_SCREEN_H_
#define _DISPENSE_TEST_SCREEN_H_

#include "../../gpu/App_Common.h"
#include "../../gpu/Platform.h"
#include "../../Constants.h"

// Dispense test screen parameters
struct DispenseTestScreenParams {
  const char* statusMessage;
};

/**
 * @brief Display the dispense test screen
 * 
 * Creates a test interface with:
 * - Row 1: Dispense button
 * - Row 2: Vibration level selection (U0-U4)
 * - Row 3: Vibration time selection (1-5s)
 * - Back button
 * 
 * @param phost Pointer to GPU HAL context
 * @param params Screen parameters including status message
 */
void drawDispenseTestScreen(Gpu_Hal_Context_t *phost, const DispenseTestScreenParams& params);

#endif /* _DISPENSE_TEST_SCREEN_H_ */

/**
 * @brief Display the dispense test screen
 * 
 * Creates a test interface for dispense functionality testing
 * 
 * @param phost Pointer to GPU HAL context
 * @param params Screen parameters including status message
 */
void drawDispenseTestScreen(Gpu_Hal_Context_t *phost, const DispenseTestScreenParams& params)
{
    char buf[100];
    
    // Initialize display
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
    Gpu_CoCmd_Text(phost, DispWidth/2, 20, 28, OPT_CENTER | OPT_FORMAT, "Dispense Test");
    
    // Draw separator line
    App_WrCoCmd_Buffer(phost, COLOR_RGB(193, 64, 0));
    App_WrCoCmd_Buffer(phost, BEGIN(LINES));
    App_WrCoCmd_Buffer(phost, VERTEX2F(0, 50*16));  // Convert to 1/16 pixel units
    App_WrCoCmd_Buffer(phost, VERTEX2F(DispWidth*16, 50*16));
    App_WrCoCmd_Buffer(phost, END());
    
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // Calculate layout dimensions
    int32_t row_height = 40;
    int32_t start_y = 65;
    int32_t button_width = 45;
    int32_t button_height = 25;
    int32_t button_spacing = 6;
    
    // Row 1: Dispense button (centered, larger)
    int32_t dispense_width = 80;
    int32_t dispense_height = 30;
    int32_t dispense_x = (DispWidth - dispense_width) / 2;
    int32_t dispense_y = start_y;
    
    Gpu_CoCmd_FgColor(phost, 0xFF4500);  // Orange red for dispense
    App_WrCoCmd_Buffer(phost, TAG(TAG_DISPENSE));
    Gpu_CoCmd_Button(phost, dispense_x, dispense_y, dispense_width, dispense_height, 22, 0, "DISPENSE");
    
    // Row 2: Vibration Level Selection (U0-U4)
    int32_t vib_level_y = start_y + row_height + 15;
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // Calculate starting position for centered buttons
    int32_t total_vib_width = (5 * button_width) + (4 * button_spacing);
    int32_t vib_start_x = (DispWidth - total_vib_width) / 2;
    
    // U0 button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U0));
    Gpu_CoCmd_Button(phost, vib_start_x, vib_level_y, button_width, button_height, 20, 0, "U0");
    
    // U1 button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U1));
    Gpu_CoCmd_Button(phost, vib_start_x + (button_width + button_spacing), vib_level_y, button_width, button_height, 20, 0, "U1");
    
    // U2 button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U2));
    Gpu_CoCmd_Button(phost, vib_start_x + 2*(button_width + button_spacing), vib_level_y, button_width, button_height, 20, 0, "U2");
    
    // U3 button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U3));
    Gpu_CoCmd_Button(phost, vib_start_x + 3*(button_width + button_spacing), vib_level_y, button_width, button_height, 20, 0, "U3");
    
    // U4 button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_U4));
    Gpu_CoCmd_Button(phost, vib_start_x + 4*(button_width + button_spacing), vib_level_y, button_width, button_height, 20, 0, "U4");
    
    // Row 3: Vibration Time Selection (1-5s)
    int32_t vib_time_y = vib_level_y + row_height + 5;
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // Calculate starting position for centered buttons
    int32_t total_time_width = (5 * button_width) + (4 * button_spacing);
    int32_t time_start_x = (DispWidth - total_time_width) / 2;
    
    // 1s button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_1));
    Gpu_CoCmd_Button(phost, time_start_x, vib_time_y, button_width, button_height, 20, 0, "1s");
    
    // 2s button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_2));
    Gpu_CoCmd_Button(phost, time_start_x + (button_width + button_spacing), vib_time_y, button_width, button_height, 20, 0, "2s");
    
    // 3s button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_3));
    Gpu_CoCmd_Button(phost, time_start_x + 2*(button_width + button_spacing), vib_time_y, button_width, button_height, 20, 0, "3s");
    
    // 4s button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_4));
    Gpu_CoCmd_Button(phost, time_start_x + 3*(button_width + button_spacing), vib_time_y, button_width, button_height, 20, 0, "4s");
    
    // 5s button
    Gpu_CoCmd_FgColor(phost, 0x666666);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIB_TIME_5));
    Gpu_CoCmd_Button(phost, time_start_x + 4*(button_width + button_spacing), vib_time_y, button_width, button_height, 20, 0, "5s");
    
    // Back button
    Gpu_CoCmd_FgColor(phost, 0xAA0000);
    App_WrCoCmd_Buffer(phost, TAG(TAG_DISPENSE_BACK));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Button(phost, 10, DispHeight - 30, 50, 22, 20, 0, "Back");
    
    // Status message display
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(200, 200, 200));
    Gpu_CoCmd_Text(phost, DispWidth - 10, DispHeight - 15, 20, OPT_RIGHTX | OPT_FORMAT, params.statusMessage);
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    
    // Finalize display
    Disp_End(phost);
}

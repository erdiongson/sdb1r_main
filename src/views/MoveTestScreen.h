/*
 * MoveTestScreen.h
 * 
 * Manual movement test screen for SDB-1R
 * Provides directional controls for testing movement functionality
 * 
 * Author: XentiQ
 * Created: 2024-09-30
 */

#ifndef _MOVE_TEST_SCREEN_H_
#define _MOVE_TEST_SCREEN_H_

#include "../gpu/App_Common.h"
#include "../gpu/Platform.h"
#include "../Constants.h"

// Limit switch states structure
struct LimitSwitchStates {
  bool x_max_limit;
  bool x_min_limit;
  bool y_max_limit;
  bool y_min_limit;
  bool z_max_limit;
  bool z_min_limit;
};

/**
 * @brief Display the movement test screen
 * 
 * Creates a test interface with:
 * - Left 2/3: Square directional buttons (Up, Down, Left, Right)
 * - Right 1/3: Z-axis controls (Up, Down)
 * 
 * @param phost Pointer to GPU HAL context
 * @param limitStates Limit switch states for display
 */
void draw_move_test_screen(Gpu_Hal_Context_t *phost, const LimitSwitchStates& limitStates);

#endif /* _MOVE_TEST_SCREEN_H_ */

/**
 * @brief Display the movement test screen
 * 
 * Creates a test interface with directional controls for manual movement testing
 * 
 * @param phost Pointer to GPU HAL context
 * @param limitStates Limit switch states for display
 */
void draw_move_test_screen(Gpu_Hal_Context_t *phost, const LimitSwitchStates& limitStates)
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
    Gpu_CoCmd_Text(phost, DispWidth/2, 20, 30, OPT_CENTER | OPT_FORMAT, "Movement Test");
    
    // Draw separator line
    App_WrCoCmd_Buffer(phost, COLOR_RGB(193, 64, 0));
    App_WrCoCmd_Buffer(phost, BEGIN(LINES));
    App_WrCoCmd_Buffer(phost, VERTEX2F(0, 50*16));  // Convert to 1/16 pixel units
    App_WrCoCmd_Buffer(phost, VERTEX2F(DispWidth*16, 50*16));
    App_WrCoCmd_Buffer(phost, END());
    
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // Calculate dimensions for left 2/3 and right 1/3 sections
    int32_t left_section_width = (DispWidth * 2) / 3;
    int32_t right_section_width = DispWidth / 3;
    int32_t right_section_start = left_section_width;
    
    // Button dimensions for directional controls
    int32_t button_size = 45;
    int32_t center_x = left_section_width / 2;
    int32_t center_y = (DispHeight + 60) / 2;  // Offset from header
    
    // Left section - XY Movement Controls
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Text(phost, center_x, 70, 26, OPT_CENTER | OPT_FORMAT, "XY Movement");
    
    // UP button
    Gpu_CoCmd_FgColor(phost, 0x006400);
    App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_UP));
    Gpu_CoCmd_Button(phost, center_x - button_size/2, center_y - button_size - 10, 
                     button_size, button_size, 26, 0, "UP");
    
    // UP button limit switch dot (Y max limit)
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.y_max_limit ? 255 : 128, limitStates.y_max_limit ? 0 : 128, limitStates.y_max_limit ? 0 : 128));
    App_WrCoCmd_Buffer(phost, POINT_SIZE(4*16));
    App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
    App_WrCoCmd_Buffer(phost, VERTEX2F((center_x - button_size/2 + 8)*16, (center_y - button_size - 2)*16));
    App_WrCoCmd_Buffer(phost, END());
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // DOWN button
    Gpu_CoCmd_FgColor(phost, 0x006400);
    App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_DOWN));
    Gpu_CoCmd_Button(phost, center_x - button_size/2, center_y + 10, 
                     button_size, button_size, 26, 0, "DOWN");
    
    // DOWN button limit switch dot (Y min limit)
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.y_min_limit ? 255 : 128, limitStates.y_min_limit ? 0 : 128, limitStates.y_min_limit ? 0 : 128));
    App_WrCoCmd_Buffer(phost, POINT_SIZE(4*16));
    App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
    App_WrCoCmd_Buffer(phost, VERTEX2F((center_x - button_size/2 + 8)*16, (center_y + 18)*16));
    App_WrCoCmd_Buffer(phost, END());
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // LEFT button
    Gpu_CoCmd_FgColor(phost, 0x006400);
    App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_LEFT));
    Gpu_CoCmd_Button(phost, center_x - button_size - 40, center_y - button_size/2, 
                     button_size, button_size, 26, 0, "LEFT");
    
    // LEFT button limit switch dot (X max limit)
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.x_max_limit ? 255 : 128, limitStates.x_max_limit ? 0 : 128, limitStates.x_max_limit ? 0 : 128));
    App_WrCoCmd_Buffer(phost, POINT_SIZE(4*16));
    App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
    App_WrCoCmd_Buffer(phost, VERTEX2F((center_x - button_size - 32)*16, (center_y - button_size/2 + 8)*16));
    App_WrCoCmd_Buffer(phost, END());
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // RIGHT button
    Gpu_CoCmd_FgColor(phost, 0x006400);
    App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_RIGHT));
    Gpu_CoCmd_Button(phost, center_x + 40, center_y - button_size/2, 
                     button_size, button_size, 26, 0, "RIGHT");
    
    // RIGHT button limit switch dot (X min limit)
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.x_min_limit ? 255 : 128, limitStates.x_min_limit ? 0 : 128, limitStates.x_min_limit ? 0 : 128));
    App_WrCoCmd_Buffer(phost, POINT_SIZE(4*16));
    App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
    App_WrCoCmd_Buffer(phost, VERTEX2F((center_x + 48)*16, (center_y - button_size/2 + 8)*16));
    App_WrCoCmd_Buffer(phost, END());
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // Right section - Z Movement Controls
    int32_t z_center_x = right_section_start + right_section_width / 2;
    int32_t z_button_width = right_section_width - 20;  // Leave some margin
    int32_t z_button_height = 35;
    
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Text(phost, z_center_x, 70, 26, OPT_CENTER | OPT_FORMAT, "Z Axis");
    
    // Z UP button
    Gpu_CoCmd_FgColor(phost, 0x0066CC);
    App_WrCoCmd_Buffer(phost, TAG(TAG_Z_UP));
    Gpu_CoCmd_Button(phost, right_section_start + 10, center_y - z_button_height - 10, 
                     z_button_width, z_button_height, 26, 0, "Z UP");
    
    // Z UP button limit switch dot (Z min limit)
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.z_min_limit ? 255 : 128, limitStates.z_min_limit ? 0 : 128, limitStates.z_min_limit ? 0 : 128));
    App_WrCoCmd_Buffer(phost, POINT_SIZE(4*16));
    App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
    App_WrCoCmd_Buffer(phost, VERTEX2F((right_section_start + 18)*16, (center_y - z_button_height - 2)*16));
    App_WrCoCmd_Buffer(phost, END());
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // Z DOWN button
    Gpu_CoCmd_FgColor(phost, 0x0066CC);
    App_WrCoCmd_Buffer(phost, TAG(TAG_Z_DOWN));
    Gpu_CoCmd_Button(phost, right_section_start + 10, center_y + 10, 
                     z_button_width, z_button_height, 26, 0, "Z DOWN");
    
    // Z DOWN button limit switch dot (Z max limit)
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.z_max_limit ? 255 : 128, limitStates.z_max_limit ? 0 : 128, limitStates.z_max_limit ? 0 : 128));
    App_WrCoCmd_Buffer(phost, POINT_SIZE(4*16));
    App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
    App_WrCoCmd_Buffer(phost, VERTEX2F((right_section_start + 18)*16, (center_y + 18)*16));
    App_WrCoCmd_Buffer(phost, END());
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
    
    // Draw vertical separator line between sections
    App_WrCoCmd_Buffer(phost, COLOR_RGB(128, 128, 128));
    App_WrCoCmd_Buffer(phost, BEGIN(LINES));
    App_WrCoCmd_Buffer(phost, VERTEX2F(right_section_start*16, 60*16));
    App_WrCoCmd_Buffer(phost, VERTEX2F(right_section_start*16, (DispHeight-40)*16));
    App_WrCoCmd_Buffer(phost, END());
    
    // Back button
    Gpu_CoCmd_FgColor(phost, 0xAA0000);
    App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_BACK));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_Button(phost, 10, DispHeight - 35, 60, 25, 26, 0, "Back");
    
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    
    // Finalize display
    Disp_End(phost);
}

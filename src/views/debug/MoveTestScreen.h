/*
 * MoveTestScreen.h
 *
 * Manual movement test screen for SDB-1R
 * Provides directional controls for testing movement functionality
 *
 * Author: XentiQ
 * Created: 2024-09-30
 */

#pragma once

#include "../../gpu/App_Common.h"
#include "../../gpu/Platform.h"
#include "../../Constants.h"
#include "../common/ToggleButton.h"

// Limit switch states structure.
struct LimitSwitchStates {
  bool x_max_limit;
  bool x_min_limit;
  bool y_max_limit;
  bool y_min_limit;
  bool z_max_limit;
  bool z_min_limit;
};

// Movement test control parameters.
struct MoveTestParams {
  float xy_distance_cm;
  float z_distance_cm;
  int bounce_count;
  int current_bounce_count;
  int total_bounce_count;
  bool blocking;
};

// Display the movement test screen.
// Creates a test interface with:
// - Left 2/3: Square directional buttons (Up, Down, Left, Right)
// - Right 1/3: Z-axis controls (Up, Down)
// @param phost Pointer to GPU HAL context.
// @param limitStates Limit switch states for display.
// @param params Movement control parameters.
void drawMoveTestScreen(Gpu_Hal_Context_t* phost, const LimitSwitchStates& limitStates, const MoveTestParams& params);

// Display the movement test screen.
// Creates a test interface with directional controls for manual movement testing.
// @param phost Pointer to GPU HAL context.
// @param limitStates Limit switch states for display.
// @param params Movement control parameters.
void drawMoveTestScreen(Gpu_Hal_Context_t* phost, const LimitSwitchStates& limitStates, const MoveTestParams& params) {
  char buf[100];
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
  Gpu_CoCmd_Text(phost, DispWidth / 2, 15, 28, OPT_CENTER | OPT_FORMAT, "Movement Test");

  // Draw separator line
  App_WrCoCmd_Buffer(phost, COLOR_RGB(193, 64, 0));
  App_WrCoCmd_Buffer(phost, BEGIN(LINES));
  App_WrCoCmd_Buffer(phost, VERTEX2F(0, 35 * 16));
  App_WrCoCmd_Buffer(phost, VERTEX2F(DispWidth * 16, 35 * 16));
  App_WrCoCmd_Buffer(phost, END());

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Control buttons below title section
  int32_t control_y = 45;
  int32_t control_x = 10;
  int32_t control_width = 55;
  int32_t control_height = 25;
  int32_t control_spacing = 6;

  // XY Distance button
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_XY_DIST));
  int xy_int = (int)params.xy_distance_cm;
  int xy_dec = (int)((params.xy_distance_cm - xy_int) * 10);
  sprintf(buf, "XY:%d.%d", xy_int, xy_dec);
  Gpu_CoCmd_Button(phost, control_x, control_y, control_width, control_height, 20, 0, buf);

  // Z Distance button
  control_x += control_width + control_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_Z_DIST));
  int z_int = (int)params.z_distance_cm;
  int z_dec = (int)((params.z_distance_cm - z_int) * 10);
  sprintf(buf, "Z:%d.%d", z_int, z_dec);
  Gpu_CoCmd_Button(phost, control_x, control_y, control_width, control_height, 20, 0, buf);

  // Repeat button
  control_x += control_width + control_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_BOUNCE));
  sprintf(buf, "Repeat:%d", params.bounce_count);
  Gpu_CoCmd_Button(phost, control_x, control_y, 70, control_height, 20, 0, buf);

  // Blocking toggle (right of Repeat button)
  control_x += 70 + control_spacing;
  Toggle_Button(phost, params.blocking, TAG_MOVE_BLOCKING, control_x + 15, control_y + 6, "BLK", "BLK");
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));  // Reset text color to white

  // Calculate dimensions for left 2/3 and right 1/3 sections
  int32_t left_section_width = (DispWidth * 2) / 3;
  int32_t right_section_width = DispWidth / 3;
  int32_t right_section_start = left_section_width;

  // Button dimensions for directional controls
  int32_t button_size = 45;
  int32_t center_x = left_section_width / 2;
  int32_t center_y = (DispHeight + 45) / 2 - 10;  // Moved down to accommodate controls at top

  // UP button
  Gpu_CoCmd_FgColor(phost, 0x006400);
  App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_UP));
  Gpu_CoCmd_Button(phost, center_x - button_size / 2, center_y - button_size - 10, button_size, button_size, 26, 0,
                   "UP");

  // UP button limit switch dot (Y max limit)
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.y_max_limit ? 255 : 128, limitStates.y_max_limit ? 0 : 128,
                                      limitStates.y_max_limit ? 0 : 128));
  App_WrCoCmd_Buffer(phost, POINT_SIZE(4 * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F((center_x - button_size / 2 + 8) * 16, (center_y - button_size - 2) * 16));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // DOWN button
  Gpu_CoCmd_FgColor(phost, 0x006400);
  App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_DOWN));
  Gpu_CoCmd_Button(phost, center_x - button_size / 2, center_y + 10, button_size, button_size, 26, 0, "DOWN");

  // DOWN button limit switch dot (Y min limit)
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.y_min_limit ? 255 : 128, limitStates.y_min_limit ? 0 : 128,
                                      limitStates.y_min_limit ? 0 : 128));
  App_WrCoCmd_Buffer(phost, POINT_SIZE(4 * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F((center_x - button_size / 2 + 8) * 16, (center_y + 18) * 16));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // LEFT button
  Gpu_CoCmd_FgColor(phost, 0x006400);
  App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_LEFT));
  Gpu_CoCmd_Button(phost, center_x - button_size - 40, center_y - button_size / 2, button_size, button_size, 26, 0,
                   "LEFT");

  // LEFT button limit switch dot (X max limit)
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.x_max_limit ? 255 : 128, limitStates.x_max_limit ? 0 : 128,
                                      limitStates.x_max_limit ? 0 : 128));
  App_WrCoCmd_Buffer(phost, POINT_SIZE(4 * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F((center_x - button_size - 32) * 16, (center_y - button_size / 2 + 8) * 16));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // RIGHT button
  Gpu_CoCmd_FgColor(phost, 0x006400);
  App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_RIGHT));
  Gpu_CoCmd_Button(phost, center_x + 40, center_y - button_size / 2, button_size, button_size, 26, 0, "RIGHT");

  // RIGHT button limit switch dot (X min limit)
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.x_min_limit ? 255 : 128, limitStates.x_min_limit ? 0 : 128,
                                      limitStates.x_min_limit ? 0 : 128));
  App_WrCoCmd_Buffer(phost, POINT_SIZE(4 * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F((center_x + 48) * 16, (center_y - button_size / 2 + 8) * 16));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Right section - Z Movement Controls
  int32_t z_center_x = right_section_start + right_section_width / 2;
  int32_t z_button_width = right_section_width - 20;  // Leave some margin
  int32_t z_button_height = 35;
  int32_t z_button_spacing = 5;  // Spacing between buttons

  // Z UP button (aligned with UP button)
  int32_t z_start_y = center_y - button_size - 10;  // Same Y position as UP button
  Gpu_CoCmd_FgColor(phost, 0x0066CC);
  App_WrCoCmd_Buffer(phost, TAG(TAG_Z_UP));
  Gpu_CoCmd_Button(phost, right_section_start + 10, z_start_y, z_button_width, z_button_height,
                   26, 0, "Z UP");

  // Z UP button limit switch dot (Z max limit)
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.z_max_limit ? 255 : 128, limitStates.z_max_limit ? 0 : 128,
                                      limitStates.z_max_limit ? 0 : 128));
  App_WrCoCmd_Buffer(phost, POINT_SIZE(4 * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F((right_section_start + 18) * 16, (z_start_y + 8) * 16));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Z DOWN button
  Gpu_CoCmd_FgColor(phost, 0x0066CC);
  App_WrCoCmd_Buffer(phost, TAG(TAG_Z_DOWN));
  Gpu_CoCmd_Button(phost, right_section_start + 10, z_start_y + z_button_height + z_button_spacing, z_button_width, z_button_height, 26, 0, "Z DOWN");

  // Z DOWN button limit switch dot (Z min limit)
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(limitStates.z_min_limit ? 255 : 128, limitStates.z_min_limit ? 0 : 128,
                                      limitStates.z_min_limit ? 0 : 128));
  App_WrCoCmd_Buffer(phost, POINT_SIZE(4 * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
  App_WrCoCmd_Buffer(phost, VERTEX2F((right_section_start + 18) * 16, (z_start_y + z_button_height + z_button_spacing + 8) * 16));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // STOP button (below Z DOWN)
  Gpu_CoCmd_FgColor(phost, 0xFF0000);
  App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_STOP));
  Gpu_CoCmd_Button(phost, right_section_start + 10, z_start_y + (z_button_height + z_button_spacing) * 2, z_button_width, z_button_height, 26, 0, "STOP");

  // Back button (top left, vertically aligned with title)
  Gpu_CoCmd_FgColor(phost, 0xAA0000);
  App_WrCoCmd_Buffer(phost, TAG(TAG_MOVE_BACK));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 10, 4, 50, 22, 20, 0, "Back");

  // Repeat progress text at bottom center (if repeating is active)
  if (params.total_bounce_count > 0) {
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(200, 200, 200));
    sprintf(buf, "Repeating %d/%d", params.current_bounce_count, params.total_bounce_count);
    Gpu_CoCmd_Text(phost, DispWidth / 2, DispHeight - 10, 20, OPT_CENTER, buf);
    App_WrCoCmd_Buffer(phost, TAG_MASK(255));
  }

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Finalize display
  Disp_End(phost);
}

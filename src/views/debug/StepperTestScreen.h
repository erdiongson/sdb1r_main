/*
 * StepperTestScreen.h
 *
 * Stepper motor test screen for SDB-1R
 * Provides controls for testing and adjusting motor speed and acceleration
 *
 * Author: XentiQ
 * Created: 2025-10-14
 */

#pragma once

#include "../../gpu/App_Common.h"
#include "../../gpu/Platform.h"
#include "../../Constants.h"
#include "../common/ToggleButton.h"

// Axis selection enumeration.
enum StepperTestAxis {
  STEPPER_AXIS_X = 0,
  STEPPER_AXIS_Y = 1,
  STEPPER_AXIS_Z = 2
};

// Stepper test parameters.
struct StepperTestParams {
  StepperTestAxis selected_axis;
  float max_speed;
  float max_acceleration;
  float move_amount_cm;
  bool blocking;
  const char* status_message;
};

// Display the stepper test screen.
// Creates a test interface with:
// - Line 1: Axis selection buttons (X, Y, Z)
// - Line 2: Max speed display and adjustment buttons
// - Line 3: Max acceleration display and adjustment buttons
// - Line 4: Horizontal divider
// - Line 5: Movement controls (-, amount input, +)
// @param phost Pointer to GPU HAL context.
// @param params Stepper test parameters.
void drawStepperTestScreen(Gpu_Hal_Context_t* phost, const StepperTestParams& params);

// Display the stepper test screen.
// Creates a test interface for adjusting motor parameters and testing movement.
// @param phost Pointer to GPU HAL context.
// @param params Stepper test parameters.
inline void drawStepperTestScreen(Gpu_Hal_Context_t* phost, const StepperTestParams& params) {
  char temp_buffer[20];  // Max: "100000" or "99.9cm"
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
  Gpu_CoCmd_Text(phost, DispWidth / 2, 15, 28, OPT_CENTER | OPT_FORMAT, "Stepper Test");

  // Draw separator line
  App_WrCoCmd_Buffer(phost, COLOR_RGB(193, 64, 0));
  App_WrCoCmd_Buffer(phost, BEGIN(LINES));
  App_WrCoCmd_Buffer(phost, VERTEX2F(0, 35 * 16));
  App_WrCoCmd_Buffer(phost, VERTEX2F(DispWidth * 16, 35 * 16));
  App_WrCoCmd_Buffer(phost, END());

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Line 1: Axis selection
  int32_t line1_y = 45;
  int32_t label_x = 5;
  int32_t button_start_x = 45;
  int32_t button_width = 45;
  int32_t button_height = 25;
  int32_t button_spacing = 6;

  // "Axis:" label
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  Gpu_CoCmd_Text(phost, label_x, line1_y + button_height / 2, 20, OPT_CENTERY, "Axis:");
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // X button
  Gpu_CoCmd_FgColor(phost, params.selected_axis == STEPPER_AXIS_X ? 0x0066CC : 0x808080);
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_AXIS_X));
  Gpu_CoCmd_Button(phost, button_start_x, line1_y, button_width, button_height, 20, 0, "X");

  // Y button
  Gpu_CoCmd_FgColor(phost, params.selected_axis == STEPPER_AXIS_Y ? 0x0066CC : 0x808080);
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_AXIS_Y));
  Gpu_CoCmd_Button(phost, button_start_x + button_width + button_spacing, line1_y, button_width, button_height, 20, 0, "Y");

  // Z button
  Gpu_CoCmd_FgColor(phost, params.selected_axis == STEPPER_AXIS_Z ? 0x0066CC : 0x808080);
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_AXIS_Z));
  Gpu_CoCmd_Button(phost, button_start_x + (button_width + button_spacing) * 2, line1_y, button_width, button_height, 20, 0, "Z");

  // Blocking toggle on the right side of the first row (right-aligned)
  int32_t toggle_width = 40;  // Width of toggle button
  int32_t blocking_x = DispWidth - toggle_width - 10;  // Right-aligned with padding
  Toggle_Button(phost, params.blocking, TAG_STEPPER_BLOCKING, blocking_x, line1_y + 2, "BLK", "BLK");
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));  // Reset text color to white

  // Line 2: Max Speed
  int32_t line2_y = line1_y + button_height + 4;
  int32_t value_button_width = 40;
  int32_t value_button_spacing = 4;
  int32_t value_display_width = 45;  // Increased width for value buttons
  int32_t adjustment_buttons_x = 100;  // Alignment point for adjustment buttons

  // "MaxSp:" label with clickable value button
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  Gpu_CoCmd_Text(phost, label_x, line2_y + button_height / 2, 20, OPT_CENTERY, "MaxSp:");
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Clickable speed value button (aligned to adjustment_buttons_x)
  sprintf(temp_buffer, "%ld", (long)params.max_speed);
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_SPEED_VALUE));
  Gpu_CoCmd_Button(phost, adjustment_buttons_x - value_display_width - 5, line2_y, value_display_width, button_height, 20, 0, buf);

  // Speed adjustment buttons - Row 1: -10k, +10k, -1k, +1k
  int32_t speed_button_x = adjustment_buttons_x;
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);

  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_SPEED_MINUS_10K));
  Gpu_CoCmd_Button(phost, speed_button_x, line2_y, value_button_width, button_height, 20, 0, "-10k");

  speed_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_SPEED_PLUS_10K));
  Gpu_CoCmd_Button(phost, speed_button_x, line2_y, value_button_width, button_height, 20, 0, "+10k");

  speed_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_SPEED_MINUS_1K));
  Gpu_CoCmd_Button(phost, speed_button_x, line2_y, value_button_width, button_height, 20, 0, "-1k");

  speed_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_SPEED_PLUS_1K));
  Gpu_CoCmd_Button(phost, speed_button_x, line2_y, value_button_width, button_height, 20, 0, "+1k");

  // Speed adjustment buttons - Row 2: -100, +100, Reset
  int32_t line2b_y = line2_y + button_height + 4;
  speed_button_x = adjustment_buttons_x;

  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_SPEED_MINUS_100));
  Gpu_CoCmd_Button(phost, speed_button_x, line2b_y, value_button_width, button_height, 20, 0, "-100");

  speed_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_SPEED_PLUS_100));
  Gpu_CoCmd_Button(phost, speed_button_x, line2b_y, value_button_width, button_height, 20, 0, "+100");

  speed_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_SPEED_RESET));
  Gpu_CoCmd_Button(phost, speed_button_x, line2b_y, value_button_width + 5, button_height, 20, 0, "Reset");

  // Line 3: Max Acceleration
  int32_t line3_y = line2b_y + button_height + 4;

  // "MaxAcc:" label with clickable value button
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  Gpu_CoCmd_Text(phost, label_x, line3_y + button_height / 2, 20, OPT_CENTERY, "MaxAcc:");
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Clickable acceleration value button (aligned to adjustment_buttons_x)
  sprintf(temp_buffer, "%ld", (long)params.max_acceleration);
  Gpu_CoCmd_FgColor(phost, 0x666666);
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_ACCEL_VALUE));
  Gpu_CoCmd_Button(phost, adjustment_buttons_x - value_display_width - 5, line3_y, value_display_width, button_height, 20, 0, temp_buffer);

  // Acceleration adjustment buttons - Row 1: -10k, +10k, -1k, +1k
  int32_t accel_button_x = adjustment_buttons_x;
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);

  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_ACCEL_MINUS_10K));
  Gpu_CoCmd_Button(phost, accel_button_x, line3_y, value_button_width, button_height, 20, 0, "-10k");

  accel_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_ACCEL_PLUS_10K));
  Gpu_CoCmd_Button(phost, accel_button_x, line3_y, value_button_width, button_height, 20, 0, "+10k");

  accel_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_ACCEL_MINUS_1K));
  Gpu_CoCmd_Button(phost, accel_button_x, line3_y, value_button_width, button_height, 20, 0, "-1k");

  accel_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_ACCEL_PLUS_1K));
  Gpu_CoCmd_Button(phost, accel_button_x, line3_y, value_button_width, button_height, 20, 0, "+1k");

  // Acceleration adjustment buttons - Row 2: -100, +100, Reset
  int32_t line3b_y = line3_y + button_height + 4;
  accel_button_x = adjustment_buttons_x;

  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_ACCEL_MINUS_100));
  Gpu_CoCmd_Button(phost, accel_button_x, line3b_y, value_button_width, button_height, 20, 0, "-100");

  accel_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_ACCEL_PLUS_100));
  Gpu_CoCmd_Button(phost, accel_button_x, line3b_y, value_button_width, button_height, 20, 0, "+100");

  accel_button_x += value_button_width + value_button_spacing;
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_ACCEL_RESET));
  Gpu_CoCmd_Button(phost, accel_button_x, line3b_y, value_button_width + 5, button_height, 20, 0, "Reset");

  // Line 4: Horizontal divider
  int32_t divider_y = line3b_y + button_height + 10;
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(128, 128, 128));
  App_WrCoCmd_Buffer(phost, BEGIN(LINES));
  App_WrCoCmd_Buffer(phost, VERTEX2F(20 * 16, divider_y * 16));
  App_WrCoCmd_Buffer(phost, VERTEX2F((DispWidth - 20) * 16, divider_y * 16));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  // Line 5: Movement controls (right-aligned, same size as other buttons)
  int32_t line5_y = divider_y + 10;
  int32_t move_button_width = 40;
  int32_t move_button_height = 25;
  int32_t amount_button_width = 50;
  int32_t button_gap = 4;

  // Calculate positions for right-aligned layout
  int32_t total_width = move_button_width + button_gap + amount_button_width + button_gap + move_button_width;
  int32_t start_x = DispWidth - total_width - 10;  // 10px margin from right edge

  // "-" button
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_MOVE_MINUS));
  Gpu_CoCmd_Button(phost, start_x, line5_y, move_button_width, move_button_height, 20, 0, "-");

  // Amount input button
  start_x += move_button_width + button_gap;
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_MOVE_AMOUNT));
  int amount_int = (int)params.move_amount_cm;
  int amount_dec = (int)((params.move_amount_cm - amount_int) * 10);
  sprintf(temp_buffer, "%d.%dcm", amount_int, amount_dec);
  Gpu_CoCmd_Button(phost, start_x, line5_y, amount_button_width, move_button_height, 20, 0, temp_buffer);

  // "+" button
  start_x += amount_button_width + button_gap;
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_MOVE_PLUS));
  Gpu_CoCmd_Button(phost, start_x, line5_y, move_button_width, move_button_height, 20, 0, "+");

  // Back button (top left, vertically aligned with title)
  Gpu_CoCmd_FgColor(phost, 0xAA0000);
  App_WrCoCmd_Buffer(phost, TAG(TAG_STEPPER_BACK));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 10, 4, 50, 22, 20, 0, "Back");

  // Status message at bottom left
  if (params.status_message && params.status_message[0] != '\0') {
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    App_WrCoCmd_Buffer(phost, COLOR_RGB(200, 200, 200));
    Gpu_CoCmd_Text(phost, 10, DispHeight - 10, 20, 0, params.status_message);
  }

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Finalize display
  Disp_End(phost);
}

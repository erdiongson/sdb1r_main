#pragma once

#include "../../gpu/App_Common.h"
#include "../../logic/TrayPositionHandler.h"
#include "../../Constants.h"

// Parameters for Preview_Screen display
struct PreviewScreenParams {
  uint8_t grid_cols;      // Number of columns in the grid (max 42)
  uint8_t grid_rows;      // Number of rows in the grid (max 33)
  bool simulating;        // Whether simulation mode is active
  uint8_t simulate_col;   // Column being simulated (0 = none, max 42)
  uint8_t simulate_row;   // Row being simulated (0 = none, max 33)
  const char* info_text;  // Text to display in bottom right (e.g., "Preview (Grid 10x10)" or "Position = 5x3")
  bool staggered;         // If true, even rows are shifted right by half a step with last cell removed
};

/**
 * @brief Display a grid preview screen showing enabled/disabled positions
 *
 * @param phost GPU context
 * @param skip_positions Array of positions to skip (disabled)
 * @param params Preview screen parameters (grid size, simulation state)
 *
 * Grid dots are displayed as:
 * - Enabled: White outline circle with empty fill
 * - Disabled: Gray outline circle
 * - Simulated: Green circle (when simulateCol/simulateRow are set)
 *
 * The grid is automatically centered on the screen and scaled to fit
 * the maximum grid size (MAX_TUBES_X x MAX_TUBES_Y)
 */
void drawPreviewScreen(Gpu_Hal_Context_t* phost, const TrayHandler::Position skip_positions[MAX_SKIP_POSITIONS],
                       const PreviewScreenParams& params) {
  int grid_cols = params.grid_cols;
  int grid_rows = params.grid_rows;

  // Display dimensions (320x240)
  const int SCREEN_WIDTH = 320;
  const int SCREEN_HEIGHT = 240;

  // Reserve space for title and buttons
  const int TITLE_HEIGHT = 0;
  const int BUTTON_HEIGHT = 20;
  const int GRID_TOP_MARGIN = TITLE_HEIGHT + 2;
  const int GRID_BOTTOM_MARGIN = BUTTON_HEIGHT + 14;

  // Available space for grid
  const int AVAILABLE_WIDTH = SCREEN_WIDTH - 4;  // 10px margin on each side
  const int AVAILABLE_HEIGHT = SCREEN_HEIGHT - GRID_TOP_MARGIN - GRID_BOTTOM_MARGIN;

  // Calculate dot size and spacing to fit the grid
  // We need to fit TUBES_X_MAX x TUBES_Y_MAX grid
  int min_dot_spacing_x = AVAILABLE_WIDTH / (TUBES_X_MAX + 1);
  int min_dot_spacing_y = AVAILABLE_HEIGHT / (TUBES_Y_MAX + 1);

  int max_dot_spacing_x = min_dot_spacing_x * 2;
  int max_dot_spacing_y = min_dot_spacing_y * 2;

  int dot_spacing_x = AVAILABLE_WIDTH / (grid_cols + 1);
  int dot_spacing_y = AVAILABLE_HEIGHT / (grid_rows + 1);

  if (dot_spacing_x > max_dot_spacing_x) dot_spacing_x = max_dot_spacing_x;
  if (dot_spacing_y > max_dot_spacing_y) dot_spacing_y = max_dot_spacing_y;

  // Use the smaller spacing to ensure grid fits
  int dot_spacing = (dot_spacing_x < dot_spacing_y) ? dot_spacing_x : dot_spacing_y;

  // Dot radius (leave space between dots)
  int dot_radius = (dot_spacing / 2) - 1;
  if (dot_radius < 2) dot_radius = 2;  // Minimum radius
  if (dot_radius > 6) dot_radius = 6;  // Maximum radius for visibility

  // Calculate grid dimensions
  int grid_width = grid_cols * dot_spacing;
  int grid_height = grid_rows * dot_spacing;

  // Center the grid
  int grid_start_x = (SCREEN_WIDTH - grid_width) / 2;
  int grid_start_y = GRID_TOP_MARGIN + (AVAILABLE_HEIGHT - grid_height) / 2;

  // Helper lambda to check if position should be skipped
  auto is_skip_position = [&](int x, int y) -> bool {
    TrayHandler::Position pos(x, y);
    for (int i = 0; i < MAX_SKIP_POSITIONS; i++) {
      if (skip_positions[i].x == -1 || skip_positions[i].y == -1) {
        break;
      }
      // Exact position match
      if (pos.x == skip_positions[i].x && pos.y == skip_positions[i].y) {
        return true;
      }
      // Skip entire column (y == 0 means entire column)
      if (pos.x == skip_positions[i].x && skip_positions[i].y == 0) {
        return true;
      }
      // Skip entire row (x == 0 means entire row)
      if (pos.y == skip_positions[i].y && skip_positions[i].x == 0) {
        return true;
      }
    }
    return false;
  };

  // Helper lambda to check if position is being simulated
  auto is_simulated_position = [&](int x, int y) -> bool {
    return params.simulate_col != 0 && params.simulate_row != 0 && x == params.simulate_col && y == params.simulate_row;
  };

  // Start drawing
  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));

  // Title removed - now shown in bottom right grid info

  // Draw all grid positions - BATCHED for efficiency
  // Draw all enabled positions in one batch (white dots)
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, POINT_SIZE(dot_radius * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));

  int enabled_count = 0;
  for (int row = 1; row <= grid_rows; row++) {
    // Check if this is an even row and staggered mode is enabled
    bool is_even_row = (row % 2 == 0);
    int x_offset = (params.staggered && is_even_row) ? (dot_spacing / 2) : 0;
    int max_col = (params.staggered && is_even_row) ? (grid_cols - 1) : grid_cols;

    for (int col = 1; col <= max_col; col++) {
      if (!is_skip_position(col, row)) {
        int center_x = grid_start_x + (col * dot_spacing) + x_offset;
        int center_y = grid_start_y + (grid_rows - row + 1) * dot_spacing;
        App_WrCoCmd_Buffer(phost, VERTEX2F(center_x * 16, center_y * 16));
        enabled_count++;
      }
    }
  }
  App_WrCoCmd_Buffer(phost, END());

  // Draw simulated position (green dot)
  if (params.simulate_col != 0 && params.simulate_row != 0) {
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 255, 0));
    App_WrCoCmd_Buffer(phost, POINT_SIZE(dot_radius * 16));
    App_WrCoCmd_Buffer(phost, BEGIN(POINTS));

    // Apply stagger offset for even rows
    bool is_even_row = (params.simulate_row % 2 == 0);
    int x_offset = (params.staggered && is_even_row) ? (dot_spacing / 2) : 0;

    int center_x = grid_start_x + (params.simulate_col * dot_spacing) + x_offset;
    int center_y = grid_start_y + ((grid_rows - params.simulate_row + 1) * dot_spacing);
    App_WrCoCmd_Buffer(phost, VERTEX2F(center_x * 16, center_y * 16));
    App_WrCoCmd_Buffer(phost, END());
  }

  // Draw all skipped positions in one batch (gray dots)
  App_WrCoCmd_Buffer(phost, COLOR_RGB(128, 128, 128));
  App_WrCoCmd_Buffer(phost, POINT_SIZE(dot_radius * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));

  int skipped_count = 0;
  for (int row = 1; row <= grid_rows; row++) {
    // Check if this is an even row and staggered mode is enabled
    bool is_even_row = (row % 2 == 0);
    int x_offset = (params.staggered && is_even_row) ? (dot_spacing / 2) : 0;
    int max_col = (params.staggered && is_even_row) ? (grid_cols - 1) : grid_cols;

    for (int col = 1; col <= max_col; col++) {
      if (is_skip_position(col, row)) {
        int center_x = grid_start_x + (col * dot_spacing) + x_offset;
        int center_y = grid_start_y + (grid_rows - row + 1) * dot_spacing;
        App_WrCoCmd_Buffer(phost, VERTEX2F(center_x * 16, center_y * 16));
        skipped_count++;
      }
    }
  }
  App_WrCoCmd_Buffer(phost, END());

  // Draw Back button (left side)
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_PREVIEW_BACK));
  Gpu_CoCmd_Button(phost, 10, SCREEN_HEIGHT - 26 - 4, 62, 26, 21, 0, "Back");

  // Draw Simulate/Stop button (next to Back button)
  if (params.simulating) {
    Gpu_CoCmd_FgColor(phost, 0xFF0000);  // Red for Stop
    App_WrCoCmd_Buffer(phost, TAG(TAG_PREVIEW_STOP));
    Gpu_CoCmd_Button(phost, 82, SCREEN_HEIGHT - 26 - 4, 62, 26, 21, 0, "Stop");
  } else {
    Gpu_CoCmd_FgColor(phost, 0x00A2E8);  // Blue for Simulate
    App_WrCoCmd_Buffer(phost, TAG(TAG_PREVIEW_SIMULATE));
    Gpu_CoCmd_Button(phost, 82, SCREEN_HEIGHT - 26 - 4, 62, 26, 21, 0, "Simulate");
  }
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Draw grid info (bottom right, aligned with button)
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Text(phost, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 25, 21, OPT_RIGHTX, params.info_text);

  Disp_End(phost);
}

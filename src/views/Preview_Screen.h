#ifndef PREVIEW_SCREEN_H
#define PREVIEW_SCREEN_H

#include "../gpu/App_Common.h"
#include "../logic/TrayPositionHandler.h"
#include "Tags.h"

// Parameters for Preview_Screen display
struct PreviewScreenParams {
  int gridCols;      // Number of columns in the grid
  int gridRows;      // Number of rows in the grid
  bool simulating;   // Whether simulation mode is active
  int simulateCol;   // Column being simulated (0 = none)
  int simulateRow;   // Row being simulated (0 = none)
  const char* infoText; // Text to display in bottom right (e.g., "Preview (Grid 10x10)" or "Position = 5x3")
};

/**
 * @brief Display a grid preview screen showing enabled/disabled positions
 * 
 * @param phost GPU context
 * @param skipPositions Array of positions to skip (disabled)
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
void Preview_Screen(Gpu_Hal_Context_t *phost, 
                   const TrayHandler::Position skipPositions[MAX_POSITIONS],
                   const PreviewScreenParams& params) {
  
  int gridCols = params.gridCols;
  int gridRows = params.gridRows;
  
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
  // We need to fit MAX_TUBES_X x MAX_TUBES_Y grid
  int minDotSpacingX = AVAILABLE_WIDTH / (MAX_TUBES_X + 1);
  int minDotSpacingY = AVAILABLE_HEIGHT / (MAX_TUBES_Y + 1);

  int maxDotSpacingX = minDotSpacingX * 2;
  int maxDotSpacingY = minDotSpacingY * 2;
  
  int dotSpacingX = AVAILABLE_WIDTH / (gridCols + 1);
  int dotSpacingY = AVAILABLE_HEIGHT / (gridRows + 1);

  if (dotSpacingX > maxDotSpacingX) dotSpacingX = maxDotSpacingX;
  if (dotSpacingY > maxDotSpacingY) dotSpacingY = maxDotSpacingY;
  
  // Use the smaller spacing to ensure grid fits
  int dotSpacing = (dotSpacingX < dotSpacingY) ? dotSpacingX : dotSpacingY;
  
  // Dot radius (leave space between dots)
  int dotRadius = (dotSpacing / 2) - 1;
  if (dotRadius < 2) dotRadius = 2;  // Minimum radius
  if (dotRadius > 6) dotRadius = 6;  // Maximum radius for visibility
  
  // Calculate grid dimensions
  int gridWidth = gridCols * dotSpacing;
  int gridHeight = gridRows * dotSpacing;
  
  // Center the grid
  int gridStartX = (SCREEN_WIDTH - gridWidth) / 2;
  int gridStartY = GRID_TOP_MARGIN + (AVAILABLE_HEIGHT - gridHeight) / 2;
  
  // Helper lambda to check if position should be skipped
  auto isSkipPosition = [&](int x, int y) -> bool {
    TrayHandler::Position pos(x, y);
    for (int i = 0; i < MAX_POSITIONS; i++) {
      if (skipPositions[i].x == -1 || skipPositions[i].y == -1) { break; }
      // Exact position match
      if (pos.x == skipPositions[i].x && pos.y == skipPositions[i].y) { return true; }
      // Skip entire column (y == 0 means entire column)
      if (pos.x == skipPositions[i].x && skipPositions[i].y == 0) { return true; }
      // Skip entire row (x == 0 means entire row)
      if (pos.y == skipPositions[i].y && skipPositions[i].x == 0) { return true; }
    }
    return false;
  };
  
  // Helper lambda to check if position is being simulated
  auto isSimulatedPosition = [&](int x, int y) -> bool {
    return params.simulateCol != 0 && params.simulateRow != 0 &&
           x == params.simulateCol && y == params.simulateRow;
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
  App_WrCoCmd_Buffer(phost, POINT_SIZE(dotRadius * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
  
  int enabledCount = 0;
  for (int row = 1; row <= gridRows; row++) {
    for (int col = 1; col <= gridCols; col++) {
      if (!isSkipPosition(col, row)) {
        int centerX = gridStartX + (col * dotSpacing);
        int centerY = gridStartY + (gridRows - row + 1) * dotSpacing;
        App_WrCoCmd_Buffer(phost, VERTEX2F(centerX * 16, centerY * 16));
        enabledCount++;
      }
    }
  }
  App_WrCoCmd_Buffer(phost, END());
  
  // Draw simulated position (green dot)
  if (params.simulateCol != 0 && params.simulateRow != 0) {
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 255, 0));
    App_WrCoCmd_Buffer(phost, POINT_SIZE(dotRadius * 16));
    App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
    int centerX = gridStartX + (params.simulateCol * dotSpacing);
    int centerY = gridStartY + ((gridRows - params.simulateRow + 1) * dotSpacing);
    App_WrCoCmd_Buffer(phost, VERTEX2F(centerX * 16, centerY * 16));
    App_WrCoCmd_Buffer(phost, END());
  }
  
  // Draw all skipped positions in one batch (gray dots)
  App_WrCoCmd_Buffer(phost, COLOR_RGB(128, 128, 128));
  App_WrCoCmd_Buffer(phost, POINT_SIZE(dotRadius * 16));
  App_WrCoCmd_Buffer(phost, BEGIN(POINTS));
  
  int skippedCount = 0;
  for (int row = 1; row <= gridRows; row++) {
    for (int col = 1; col <= gridCols; col++) {
      if (isSkipPosition(col, row)) {
        int centerX = gridStartX + (col * dotSpacing);
        int centerY = gridStartY + (gridRows - row + 1) * dotSpacing;
        App_WrCoCmd_Buffer(phost, VERTEX2F(centerX * 16, centerY * 16));
        skippedCount++;
      }
    }
  }
  App_WrCoCmd_Buffer(phost, END());
  
  // Draw Back button (left side)
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(KEY_CONFIG_PREVIEW_BACK));
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
  Gpu_CoCmd_Text(phost, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 25, 21, OPT_RIGHTX, params.infoText);
  
  Disp_End(phost);
}

#endif // PREVIEW_SCREEN_H

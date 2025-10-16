#pragma once

#include "../../gpu/App_Common.h"
#include "../../logic/Profile.h"
#include "../../logic/TrayPositionHandler.h"
#include "../../logic/SkipUtils.h"
#include "../common/ToggleButton.h"
#include "../common/Dialogs.h"
#include "../../Constants.h"

// Parameters for whether to highlight skip fields in red
struct SkipErrors {
  bool skip_cols = false;
  bool skip_rows = false;
  bool skip_cells = false;
  bool skip_count_exceeded = false;
};

// Parameters for Advanced Settings Screen display.
struct AdvancedSettingsScreenParams {
  Profile& profile;
  SkipErrors errors;
  int dialog_code;
};

inline void drawAdvancedSettingsScreen(Gpu_Hal_Context_t* phost, AdvancedSettingsScreenParams params) {
  Profile& profile = params.profile;
  char rowBuf[SKIP_STRING_LEN];        // a buffer for skip row entry
  char colBuf[SKIP_STRING_LEN];        // a buffer for skip column entry
  char singlePosBuf[SKIP_STRING_LEN];  // a buffer for skip single position entry
  uint8_t keypressed;

  // Convert SkipPosition array to strings for display
  SkipUtils::convertToStrings(profile.skip_positions, profile.skip_count, colBuf, rowBuf, singlePosBuf);

  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 10, 12, 21, 0, "Skipped Columns:");
  Gpu_CoCmd_Text(phost, 10, 60, 21, 0, "Skipped Rows:");
  Gpu_CoCmd_Text(phost, 10, 108, 21, 0, "Skipped Single Position:");

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, BEGIN(RECTS));

  // Skipped Columns (edit name)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_SKIP_COLUMNS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(160, 880));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 496));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Skipped Rows (edit name)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_SKIP_ROWS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(160, 1664));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 1280));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Skipped Single Position (edit name)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_SKIP_SINGLE_POS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(160, 2448));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 2064));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  App_WrCoCmd_Buffer(phost, END());

  // Text - Skip Columns
  App_WrCoCmd_Buffer(phost, params.errors.skip_cols ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 15, 36, 21, 0, (const char*)colBuf);

  // Text - Skip Rows
  App_WrCoCmd_Buffer(phost, params.errors.skip_rows ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 15, 84, 21, 0, (const char*)rowBuf);

  // Text - Skip Single Positions
  App_WrCoCmd_Buffer(phost, params.errors.skip_cells ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 15, 132, 21, 0, (const char*)singlePosBuf);

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);

  // Back Button
  App_WrCoCmd_Buffer(phost, TAG(TAG_ADV_PROF_BACK));
  Gpu_CoCmd_Button(phost, 10, 196, 62, 26, 21, 0, "Back");

  // Preview Button
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_PREVIEW));
  Gpu_CoCmd_Button(phost, 247, 196, 62, 26, 21, 0, "Preview");

  // Staggered Mode Toggle
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Text(phost, 10, 165, 21, 0, "Staggered:");
  Toggle_Button(phost, profile.staggered, TAG_STAGGERED_TOGGLE, 92, 168, "On", "Off");

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Draw dialog if dialog_code is set
  if (params.dialog_code > 0) {
    drawDialog(phost, params.dialog_code);
  }

  Disp_End(phost);
}

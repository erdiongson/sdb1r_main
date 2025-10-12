#pragma once

#include "../gpu/App_Common.h"
#include "common/Dialogs.h"

// Button enable states for the main screen.
struct ButtonsEnabled {
  bool setting;
  bool start;
  bool pause;
  bool stop;
  bool show_resume;    // If true, START button shows "RESUME" instead of "START"
  bool show_homing;    // If true, STOP button shows "HOMING"
  bool show_stopping;  // If true, STOP button shows "STOPPING"
};

// Run status information.
struct RunStatus {
  uint16_t current_row;
  uint16_t current_column;
  uint16_t tubes_left;
  uint16_t filling_tube;
};

// Parameters for Home_Screen display.
struct MainScreenParams {
  Profile& profile;
  RunStatus run_status;
  int dialog_code;
};

// Draws menu buttons based on the button enable states.
// @param buttons The button enable states.
inline void drawMenuButtons(const ButtonsEnabled& buttons) {
  // Draw SETTING button
  App_WrCoCmd_Buffer(phost, TAG(SETTING));
  if (buttons.setting) {
    App_WrCoCmd_Buffer(phost, COLOR_A(255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  } else {
    App_WrCoCmd_Buffer(phost, COLOR_A(60));
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  }
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  Gpu_CoCmd_Button(phost, 208, 5, 90, 36, 28, OPT_FORMAT, "Settings");
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Draw START button
  App_WrCoCmd_Buffer(phost, TAG(START));
  if (buttons.start) {
    App_WrCoCmd_Buffer(phost, COLOR_A(255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  } else {
    App_WrCoCmd_Buffer(phost, COLOR_A(60));
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  }
  Gpu_CoCmd_FgColor(phost, 0x006400);
  if (buttons.show_resume)
    Gpu_CoCmd_Button(phost, 25, 112, 90, 36, 28, 0, "RESUME");
  else
    Gpu_CoCmd_Button(phost, 25, 112, 90, 36, 28, 0, "START");
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Draw PAUSE button
  App_WrCoCmd_Buffer(phost, TAG(PAUSE));
  if (buttons.pause) {
    App_WrCoCmd_Buffer(phost, COLOR_A(255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  } else {
    App_WrCoCmd_Buffer(phost, COLOR_A(60));
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  }
  Gpu_CoCmd_FgColor(phost, 0xADAF3C);
  Gpu_CoCmd_Button(phost, 207, 112, 90, 36, 28, 0, "PAUSE");
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Draw STOP button
  App_WrCoCmd_Buffer(phost, TAG(STOP));
  if (buttons.stop) {
    App_WrCoCmd_Buffer(phost, COLOR_A(255));
    App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  } else {
    App_WrCoCmd_Buffer(phost, COLOR_A(60));
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  }
  Gpu_CoCmd_FgColor(phost, 0xAA0000);
  if (buttons.show_homing)
    Gpu_CoCmd_Button(phost, 25, 157, 273, 36, 28, 0, "HOMING");
  else if (buttons.show_stopping)
    Gpu_CoCmd_Button(phost, 25, 157, 273, 36, 28, 0, "STOPPING");
  else
    Gpu_CoCmd_Button(phost, 25, 157, 273, 36, 28, 0, "STOP");
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
}

// Draws the base main screen with menu buttons and profile information.
// @param phost GPU context.
// @param buttons The button enable states.
// @param params Parameters containing profile and status information.
inline void drawBaseMainScreen(Gpu_Hal_Context_t* phost, const ButtonsEnabled& buttons,
                               const MainScreenParams& params) {
  char buf[100];

  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));
  //    Gpu_CoCmd_Text(phost, 160, 52, 31, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "XQ");
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(128, 128, 128));
  Gpu_CoCmd_Text(phost, 159, 80, 30, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "SDB-1R");
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(193, 64, 0));
  App_WrCoCmd_Buffer(phost, BEGIN(LINES));
  App_WrCoCmd_Buffer(phost, VERTEX2F(0, 1648));
  App_WrCoCmd_Buffer(phost, VERTEX2F(5120, 1648));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(255));

  drawMenuButtons(buttons);

  Profile& profile = params.profile;
  sprintf(buf, "Profile Name: %s", profile.profile_name);
  Gpu_CoCmd_Text(phost, 25, 195, 20, OPT_FORMAT, buf);  // OPT_CENTER | OPT_RIGHTX |
  sprintf(buf, "No. of Cycles: %d", profile.cycles);
  Gpu_CoCmd_Text(phost, 25, 208, 20, OPT_FORMAT, buf);

  sprintf(buf, "Filling tube: %d", params.run_status.filling_tube);
  Gpu_CoCmd_Text(phost, 25, 220, 20, OPT_FORMAT, buf);

  sprintf(buf, "Current Tube : R%2d C%2d", params.run_status.current_row, params.run_status.current_column);
  Gpu_CoCmd_Text(phost, 292, 208, 20, OPT_RIGHTX | OPT_FORMAT, buf);

  sprintf(buf, "Tube left : %3d", params.run_status.tubes_left);
  Gpu_CoCmd_Text(phost, 294, 220, 20, OPT_RIGHTX | OPT_FORMAT, buf);

  // INSERT DIALOG
  if (params.dialog_code > 0) {
    drawDialog(phost, params.dialog_code);
  }
  Disp_End(phost);
}

// Draws the ready screen (main menu).
// @param params Parameters containing profile and status information.
inline void drawReadyScreen(const MainScreenParams& params) {
  ButtonsEnabled buttons = { true, true, false, false, false, false, false };
  drawBaseMainScreen(phost, buttons, params);
}

// Draws the run screen.
// @param params Parameters containing profile and status information.
inline void drawRunScreen(const MainScreenParams& params) {
  ButtonsEnabled buttons = { false, false, true, true, false, false, false };
  drawBaseMainScreen(phost, buttons, params);
}

// Draws the pause screen.
// @param params Parameters containing profile and status information.
inline void drawPauseScreen(const MainScreenParams& params) {
  ButtonsEnabled buttons = { false, true, false, true, true, false, false };
  drawBaseMainScreen(phost, buttons, params);
}

// Draws the homing screen.
// @param params Parameters containing profile and status information.
inline void drawHomingScreen(const MainScreenParams& params) {
  ButtonsEnabled buttons = { false, false, false, false, false, true, false };
  drawBaseMainScreen(phost, buttons, params);
}

// Draws the stopping screen.
// @param params Parameters containing profile and status information.
inline void drawStoppingScreen(const MainScreenParams& params) {
  ButtonsEnabled buttons = { false, false, false, false, false, false, true };
  drawBaseMainScreen(phost, buttons, params);
}

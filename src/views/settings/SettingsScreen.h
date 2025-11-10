#pragma once

#include "../../gpu/App_Common.h"
#include "../../logic/Profile.h"
#include "../../Constants.h"
#include "../common/Dialogs.h"
#include "../ViewCommon.h"
#include "../common/ToggleButton.h"
#include "../../logic/TrayPositionHandler.h"

// Parameters for whether to highlight any fields in red
struct InputErrors {
  bool tubes_x = false;
  bool tubes_y = false;
  bool pitch_x = false;
  bool pitch_y = false;
  bool origin_x = false;
  bool origin_y = false;
  bool cycles = false;
  bool z_dip = false;
};

// Parameters for Config_Screen display.
struct SettingsScreenParams {
  Profile& profile;
  InputErrors errors;
  uint8_t dialog_code;  // Dialog codes are < 50
};

inline void drawSettingsScreen(Gpu_Hal_Context_t* phost, SettingsScreenParams params) {
  Profile& profile = params.profile;
  int16_t vibstatus;
  int16_t passwordStatus;
  int16_t vibtime_status;
  char* temp_buffer = g_view_temp_buffer;  // Use shared buffer (15 bytes needed)

  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Text(phost, 138, 20, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Configuration Settings")));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(193, 64, 0));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_HOME));
  Gpu_CoCmd_Button(phost, 233, 9, 76, 26, 21, 0, PROGMEM_STR(F("Home")));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  Gpu_CoCmd_Text(phost, 41, 82, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "");
  Gpu_CoCmd_Text(phost, 179, 78, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Column")));
  Gpu_CoCmd_Text(phost, 121, 78, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Row")));
  Gpu_CoCmd_Text(phost, 255, 78, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Cycles:")));
  if (!Z_DISABLED) {
    Gpu_CoCmd_Text(phost, 255, 115, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Z Dip:")));
  }
  Gpu_CoCmd_Text(phost, 51, 96, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("No. of Tube:")));
  Gpu_CoCmd_Text(phost, 51, 123, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Pitch(mm):")));
  Gpu_CoCmd_Text(phost, 46, 179, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Vibration:")));
  Gpu_CoCmd_Text(phost, 179, 165, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Duration:")));
  Gpu_CoCmd_Text(phost, 116, 165, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Level:")));
  Gpu_CoCmd_Text(phost, 51, 149, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Origin(mm):")));
  Gpu_CoCmd_Text(phost, 267, 155, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Password:")));
  //	  App_WrCoCmd_Buffer(phost, TAG_MASK(1));

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, BEGIN(RECTS));

  // profile name(edit name)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_PROFILE_NAME));
  App_WrCoCmd_Buffer(phost, VERTEX2F(160, 1008));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 704));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // No. of tube(row)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_TUBES_Y));
  App_WrCoCmd_Buffer(phost, VERTEX2F(1632, 1632));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2305, 1392));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // No. of tube(column)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_TUBES_X));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2544, 1632));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3216, 1392));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // pitch(row)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_PITCH_Y));
  App_WrCoCmd_Buffer(phost, VERTEX2F(1632, 2032));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2305, 1776));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // pitch(column)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_PITCH_X));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2544, 2032));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3216, 1776));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // origin(row)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_ORIGIN_Y));
  App_WrCoCmd_Buffer(phost, VERTEX2F(1632, 2432));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2305, 2160));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // origin(column)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_ORIGIN_X));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2544, 2432));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3216, 2160));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Cycles
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_NUM_CYCLE));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3744, 1632));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4480, 1392));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Z Dip
  if (!Z_DISABLED) {
    App_WrCoCmd_Buffer(phost, TAG_MASK(1));
    App_WrCoCmd_Buffer(phost, TAG(TAG_Z_DIP));
    App_WrCoCmd_Buffer(phost, VERTEX2F(3744, 2248));
    App_WrCoCmd_Buffer(phost, VERTEX2F(4480, 2008));
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  }

  App_WrCoCmd_Buffer(phost, END());

  // Text - Profile Name
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 150, 53, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, (const char*)profile.profile_name);

  // Password Enable/Disable
  Toggle_Button(phost, profile.password_enabled, TAG_PASSWORD_ENABLED, 242, 172, "On", "Off");

  // Text - Tube Number Row
  App_WrCoCmd_Buffer(phost, params.errors.tubes_x ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("%d"), profile.tube_no_x);
  Gpu_CoCmd_Text(phost, 180, 95, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, temp_buffer);

  // Text - Tube Number Column
  App_WrCoCmd_Buffer(phost, params.errors.tubes_y ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("%d"), profile.tube_no_y);
  Gpu_CoCmd_Text(phost, 122, 95, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, temp_buffer);
  
  // Text - Pitch Row
  App_WrCoCmd_Buffer(phost, params.errors.pitch_x ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));

  dtostrf(profile.pitch_x, 3, 1, temp_buffer);
  Gpu_CoCmd_Text(phost, 180, 119, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, temp_buffer);

  // Text - Pitch Column
  App_WrCoCmd_Buffer(phost, params.errors.pitch_y ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));

  dtostrf(profile.pitch_y, 3, 1, temp_buffer);
  Gpu_CoCmd_Text(phost, 122, 119, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, temp_buffer);

  // Text - Origin X
  App_WrCoCmd_Buffer(phost, params.errors.origin_x ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));

  dtostrf(profile.tray_origin_x, 3, 1, temp_buffer);
  Gpu_CoCmd_Text(phost, 180, 143, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, temp_buffer);

  // Text - Origin Y
  App_WrCoCmd_Buffer(phost, params.errors.origin_y ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));

  dtostrf(profile.tray_origin_y, 3, 1, temp_buffer);
  Gpu_CoCmd_Text(phost, 122, 143, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, temp_buffer);

  // Text - Cycles
  App_WrCoCmd_Buffer(phost, params.errors.cycles ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("%d"), profile.cycles);
  Gpu_CoCmd_Text(phost, 255, 95, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, temp_buffer);

  // Text - Z Dip
  if (!Z_DISABLED) {
    App_WrCoCmd_Buffer(phost, params.errors.z_dip ? COLOR_RGB(255, 0, 0) : COLOR_RGB(0, 0, 0));
    dtostrf(profile.z_dip, 3, 1, temp_buffer);
    Gpu_CoCmd_Text(phost, 255, 133, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, temp_buffer);
  }

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);

  // Load Button
  // App_WrCoCmd_Buffer(phost, TAG_MASK(disableButtons ? 0 : 1)); // Enable or disable based on the flag
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_LOAD));
  Gpu_CoCmd_Button(phost, 116, 205, 76, 26, 21, 0, PROGMEM_STR(F("Load")));

  // Save Button
  // App_WrCoCmd_Buffer(phost, TAG_MASK(disableButtons ? 0 : 1)); // Enable or disable based on the flag
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_SAVE));
  Gpu_CoCmd_Button(phost, 8, 205, 76, 26, 21, 0, PROGMEM_STR(F("Save")));

  App_WrCoCmd_Buffer(phost, TAG(TAG_ADVANCED));  // disable advanced button
  Gpu_CoCmd_Button(phost, 233, 205, 76, 26, 21, 0, PROGMEM_STR(F("Advanced")));

  // 20240903: erdiongson - add the variation of vibration U1 - U4
  vibstatus = profile.vibration_level;
  if (vibstatus == 1) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  // U1 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, PROGMEM_STR(F("1")));
  } else if (vibstatus == 2) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  // U2 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, PROGMEM_STR(F("2")));
  } else if (vibstatus == 3) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  // U3 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, PROGMEM_STR(F("3")));
  } else if (vibstatus == 4) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  // U4 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, PROGMEM_STR(F("4")));
  } else {
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_FgColor(phost, 0xA1A1A1);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  // U0 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, PROGMEM_STR(F("0")));
  }

  // 20241001: erdiongson - add the variation of Vibration Duration (1-5)s
  vibtime_status = profile.vibration_duration;
  if (vibtime_status == 3) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  // 3 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, PROGMEM_STR(F("3s")));
  } else if (vibtime_status == 4) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  // 4 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, PROGMEM_STR(F("4s")));
  } else if (vibtime_status == 5) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  // 5 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, PROGMEM_STR(F("5s")));
  } else if (vibtime_status == 1) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  // 1 Second Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, PROGMEM_STR(F("1s")));
  } else {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  // 2 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, PROGMEM_STR(F("2s")));
  }

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Draw dialog if dialog_code is set
  if (params.dialog_code > 0) {
    drawDialog(phost, params.dialog_code);
  }

  Disp_End(phost);
}

inline void confirmAdvanceSetting(Gpu_Hal_Context_t* phost) {
  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));

  Gpu_CoCmd_Text(phost, 156, 35, 28, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, PROGMEM_STR(F("Proceed Advanced Setting?")));
  App_WrCoCmd_Buffer(phost, TAG(246));
  Gpu_CoCmd_Button(phost, 179, 120, 60, 30, 28, 0, PROGMEM_STR(F("No")));
  App_WrCoCmd_Buffer(phost, TAG(247));
  Gpu_CoCmd_Button(phost, 68, 120, 60, 30, 28, 0, PROGMEM_STR(F("Yes")));

  Disp_End(phost);
}
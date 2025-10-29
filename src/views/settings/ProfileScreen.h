#pragma once

#include "../../gpu/App_Common.h"
#include "../../logic/Profile.h"
#include "../common/Dialogs.h"
#include "../../Constants.h"
#include "../ViewCommon.h"

// Parameters for Profile_Screen display.
struct ProfileParams {
  uint8_t keypressed;
  uint8_t cur_prof_num;
  PreviewProfile& profile;
  uint8_t dialog_code;  // Dialog codes are < 50
};

void drawProfileScreen(Gpu_Hal_Context_t* phost, ProfileParams params) {
  uint8_t keypressed = params.keypressed;
  uint8_t cur_prof_num = params.cur_prof_num;
  PreviewProfile& profile = params.profile;
  char* temp_buffer = g_view_temp_buffer;  // Use shared buffer (35 bytes needed)

  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));

  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(0, 0, 0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Text(phost, 84, 16, 27, 0, PROGMEM_STR(F("Profile Selector")));

  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_PROFILE_BACK));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 233, 11, 76, 26, 21, (keypressed == TAG_PROFILE_BACK) ? OPT_FLAT : 0, PROGMEM_STR(F("Back")));

  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_PROFILE_LOAD));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 8, 205, 76, 26, 21, (keypressed == TAG_PROFILE_LOAD) ? OPT_FLAT : 0, PROGMEM_STR(F("Load")));

  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_PROFILE_CHANGE_PASSWORD));  // disable advanced button
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 180, 205, 132, 26, 21, (keypressed == TAG_PROFILE_CHANGE_PASSWORD) ? OPT_FLAT : 0,
                   PROGMEM_STR(F("Change Password")));

  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_PROFILE_UP));
  // App_WrCoCmd_Buffer(phost, TAG(350)); //disbale Up
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 265, 76, 44, 29, 21, (keypressed == TAG_PROFILE_UP) ? OPT_FLAT : 0, PROGMEM_STR(F("Up")));

  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(TAG_PROFILE_DOWN));  // disbale Up and down
  // App_WrCoCmd_Buffer(phost, TAG(300)); //disbale Down and down
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 265, 121, 44, 29, 21, (keypressed == TAG_PROFILE_DOWN) ? OPT_FLAT : 0, PROGMEM_STR(F("Down")));

  App_WrCoCmd_Buffer(phost, BEGIN(RECTS));  // Profile Name Field
  App_WrCoCmd_Buffer(phost, VERTEX2F(160, 1008));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 704));

  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));

  Gpu_CoCmd_Text(phost, 160, 54, 27, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, (const char*)profile.profile_name);
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 127));

  // Print profileId
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("%d"), cur_prof_num + 1);
  Gpu_CoCmd_Text(phost, 82, 65, 21, 0, temp_buffer);
  Gpu_CoCmd_Text(phost, 8, 65, 21, 0, PROGMEM_STR(F("Profile ID:")));

  // // Print profileName
  // sprintf(buffer, "Profile Name: %s", curprof.profile_name);
  // Gpu_CoCmd_Text(phost, 84, 56, 27, 0, buffer);

  // Print Tube_No_x
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("%d"), profile.tube_no_x);
  Gpu_CoCmd_Text(phost, 82, 80, 21, 0, temp_buffer);
  Gpu_CoCmd_Text(phost, 8, 80, 21, 0, PROGMEM_STR(F("Columns:")));

  // Print Tube_No_y
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("%d"), profile.tube_no_y);
  Gpu_CoCmd_Text(phost, 82, 95, 21, 0, temp_buffer);
  Gpu_CoCmd_Text(phost, 8, 95, 21, 0, PROGMEM_STR(F("Rows:")));

  // Print pitch_x
  dtostrf(profile.pitch_y, 3, 1, temp_buffer);
  Gpu_CoCmd_Text(phost, 82, 110, 21, 0, temp_buffer);
  Gpu_CoCmd_Text(phost, 8, 110, 21, 0, PROGMEM_STR(F("Pitch(Col):")));

  // Print pitch_y
  dtostrf(profile.pitch_x, 3, 1, temp_buffer);
  Gpu_CoCmd_Text(phost, 82, 125, 21, 0, temp_buffer);
  Gpu_CoCmd_Text(phost, 8, 125, 21, 0, PROGMEM_STR(F("Pitch(Ros):")));

  // Print trayOriginX
  dtostrf(profile.tray_origin_x, 3, 1, temp_buffer);
  Gpu_CoCmd_Text(phost, 82, 140, 21, 0, temp_buffer);
  Gpu_CoCmd_Text(phost, 8, 140, 21, 0, PROGMEM_STR(F("OriginX:")));

  // Print trayOriginY
  dtostrf(profile.tray_origin_y, 3, 1, temp_buffer);
  Gpu_CoCmd_Text(phost, 82, 155, 21, 0, temp_buffer);
  Gpu_CoCmd_Text(phost, 8, 155, 21, 0, PROGMEM_STR(F("OriginY:")));

  // Print Cycles
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("%d"), profile.cycles);
  Gpu_CoCmd_Text(phost, 82, 170, 21, 0, temp_buffer);
  Gpu_CoCmd_Text(phost, 8, 170, 21, 0, PROGMEM_STR(F("Cycles:")));

  // Print z_dip
  if (!Z_DISABLED) {
    dtostrf(profile.z_dip, 3, 1, temp_buffer);
    Gpu_CoCmd_Text(phost, 82, 185, 21, 0, temp_buffer);
    Gpu_CoCmd_Text(phost, 8, 185, 21, 0, PROGMEM_STR(F("Z Dip:")));
  }

  // Print passwordEnabled
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("Password: %s"), profile.password_enabled ? "True" : "False");
  Gpu_CoCmd_Text(phost, 150, 65, 21, 0, temp_buffer);

  // Print vibrationDuration
  // 20241001 - erdiongson: Change in Vibration Duration
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("Vibration Time: %d"), profile.vibration_duration);
  Gpu_CoCmd_Text(phost, 150, 80, 21, 0, temp_buffer);

  // Print vibrationLevel
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("Vibration Level: %d"), profile.vibration_level);
  Gpu_CoCmd_Text(phost, 150, 95, 21, 0, temp_buffer);

  // Print staggered
  snprintf_P(temp_buffer, sizeof(g_view_temp_buffer), PSTR("Staggered: %s"), profile.staggered ? "True" : "False");
  Gpu_CoCmd_Text(phost, 150, 110, 21, 0, temp_buffer);

  // Draw dialog if dialog_code is set
  if (params.dialog_code > 0) {
    drawDialog(phost, params.dialog_code);
  }

  Disp_End(phost);
}
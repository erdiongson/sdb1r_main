#ifndef CONFIG_SCREEN_H
#define CONFIG_SCREEN_H

#include "../gpu/App_Common.h"
#include "../Utils.h"
#include "../views/Keyboards.h"
#include "Dialogs.h"

// Parameters for Config_Screen display.
struct ConfigParams {
  int dialog_code;
};

void draw_config_screen(Gpu_Hal_Context_t *phost, const ConfigParams* params = nullptr) {

  int16_t vibstatus;
  int16_t passwordStatus;
  int16_t vibtime_status;
  char buf[PROFILE_NAME_MAX_LEN];
  float maxval = 0;  //v204

  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, CLEAR_TAG(255));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Text(phost, 138, 20, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Configuration Settings");
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(193, 64, 0));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(6));
  Gpu_CoCmd_Button(phost, 233, 9, 76, 26, 21, 0, "Home");
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  Gpu_CoCmd_Text(phost, 41, 82, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "");
  Gpu_CoCmd_Text(phost, 179, 78, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Column");
  Gpu_CoCmd_Text(phost, 121, 78, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Row");
  Gpu_CoCmd_Text(phost, 255, 78, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Cycles:");
  Gpu_CoCmd_Text(phost, 255, 115, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Z Dip:");
  Gpu_CoCmd_Text(phost, 51, 96, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "No. of Tube:");
  Gpu_CoCmd_Text(phost, 51, 123, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Pitch(mm):");
  Gpu_CoCmd_Text(phost, 46, 179, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Vibration:");
  Gpu_CoCmd_Text(phost, 179, 168, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Duration:");
  Gpu_CoCmd_Text(phost, 116, 168, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Level:");
  Gpu_CoCmd_Text(phost, 51, 149, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Origin(mm):");
  Gpu_CoCmd_Text(phost, 267, 163, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Password:");
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
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(TAG_Z_DIP));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3744, 2248));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4480, 2008));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  App_WrCoCmd_Buffer(phost, END());

  //Text - Profile Name
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 150, 53, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, (const char *)CurProf.profileName);

  // Password Enable/Disable
  Toggle_Button(phost, CurProf.passwordEnabled, TAG_PASSWORD_ENABLED, 242, 180, "On", "Off");

  if (CurProf.Tube_No_x > MAXNUMX)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
  //v204
  //Text - Tube Number Row
  sprintf(buf, "%d", CurProf.Tube_No_x);
  Gpu_CoCmd_Text(phost, 180, 95, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  //v204
  //Text - Tube Number Column
  if (CurProf.Tube_No_y > MAXNUMY)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
  //v204
  sprintf(buf, "%d", CurProf.Tube_No_y);
  Gpu_CoCmd_Text(phost, 122, 95, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);
  //v204
  //Text - Pitch Row
  if (CurProf.Tube_No_x == 0) maxval = MAXXMM - CurProf.trayOriginX;
  else maxval = (MAXXMM - CurProf.trayOriginX) / (CurProf.Tube_No_x - 1);
  round_1_decimal(&maxval);
  Dprint("max val Pitch X =", maxval);
  if (maxval < CurProf.pitch_x)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
                                                    //v204

  dtostrf(CurProf.pitch_x, 4, 1, buf);
  Gpu_CoCmd_Text(phost, 180, 119, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  //v204
  //Text - Pitch Column
  if (CurProf.Tube_No_y == 0) maxval = MAXYMM - CurProf.trayOriginY;
  else maxval = (MAXYMM - CurProf.trayOriginY) / (CurProf.Tube_No_y - 1);
  round_1_decimal(&maxval);
  Dprint("max val Pitch Y =", maxval);
  if (maxval < CurProf.pitch_y)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
                                                    //v204

  dtostrf(CurProf.pitch_y, 4, 1, buf);
  Gpu_CoCmd_Text(phost, 122, 119, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  //v204
  //Text - Origin X
  if (CurProf.Tube_No_x == 0) maxval = MAXXMM;
  else maxval = MAXXMM - (CurProf.pitch_x * (CurProf.Tube_No_x - 1));
  round_1_decimal(&maxval);
  Dprint("max val Org X =", maxval);
  if (maxval < CurProf.trayOriginX)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
                                                    //v204

  dtostrf(CurProf.trayOriginX, 4, 1, buf);
  Gpu_CoCmd_Text(phost, 180, 143, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  //v204
  //Text - Origin Y
  if (CurProf.Tube_No_y == 0) maxval = MAXYMM;
  else maxval = MAXYMM - (CurProf.pitch_y * (CurProf.Tube_No_y - 1));
  round_1_decimal(&maxval);
  Dprint("max val Org Y =", maxval);
  if (maxval < CurProf.trayOriginY)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
                                                    //v204

  dtostrf(CurProf.trayOriginY, 4, 1, buf);
  Gpu_CoCmd_Text(phost, 122, 143, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
                                                  //Text - Cycles
  sprintf(buf, "%d", CurProf.Cycles);
  Gpu_CoCmd_Text(phost, 255, 95, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  //Text - Z Dip

  sprintf(buf, "%f", CurProf.ZDip);
  Dprint("ZDip =", CurProf.ZDip);
  dtostrf(CurProf.ZDip, 4, 1, buf);
  Gpu_CoCmd_Text(phost, 255, 133, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);

  // Load Button
  //App_WrCoCmd_Buffer(phost, TAG_MASK(disableButtons ? 0 : 1)); // Enable or disable based on the flag
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_LOAD));
  Gpu_CoCmd_Button(phost, 116, 205, 76, 26, 21, 0, "Load");

  // Save Button
  //App_WrCoCmd_Buffer(phost, TAG_MASK(disableButtons ? 0 : 1)); // Enable or disable based on the flag
  App_WrCoCmd_Buffer(phost, TAG(TAG_CONFIG_SAVE));
  Gpu_CoCmd_Button(phost, 8, 205, 76, 26, 21, 0, "Save");



  App_WrCoCmd_Buffer(phost, TAG(TAG_ADVANCED));  //disable advanced button
  Gpu_CoCmd_Button(phost, 233, 205, 76, 26, 21, 0, "Advanced");

  //20240903: erdiongson - add the variation of vibration U1 - U4
  vibstatus = CurProf.vibrationEnabled;
  if (vibstatus == 1) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  //U1 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "1");
  } else if (vibstatus == 2) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  //U2 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "2");
  } else if (vibstatus == 3) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  //U3 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "3");
  } else if (vibstatus == 4) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  //U4 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "4");
  } else {
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_FgColor(phost, 0xA1A1A1);
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_LEVEL));  //U0 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "0");
  }

  //20241001: erdiongson - add the variation of Vibration Duration (1-5)s
  vibtime_status = CurProf.vibrationDuration;
  if (vibtime_status == 3) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  //3 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "3s");
  } else if (vibtime_status == 4) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  //4 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "4s");
  } else if (vibtime_status == 5) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  //5 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "5s");
  } else if (vibtime_status == 1) {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  //1 Second Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "1s");
  } else {
    App_WrCoCmd_Buffer(phost, TAG(TAG_VIBRATION_DURATION));  //2 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "2s");
  }

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  
  // Draw dialog if dialog_code is set
  if (params && params->dialog_code > 0) {
    draw_dialog(phost, params->dialog_code);
  }
  
  Disp_End(phost);
}

void draw_skip_screen(Gpu_Hal_Context_t *phost) {
  char rowBuf[ROW_COL_MAX_LEN];        // a buffer for skip row entry
  char colBuf[ROW_COL_MAX_LEN];        // a buffer for skip column entry
  char singlePosBuf[ROW_COL_MAX_LEN];  // a buffer for skip single position entry
  uint8_t keypressed;

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
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 15, 36, 21, 0, (const char *)CurProf.skipCol);

  // Text - Skip Rowa
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 15, 84, 21, 0, (const char *)CurProf.skipRow);

  // Text - Skip Columns
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 15, 132, 21, 0, (const char *)CurProf.skipSinglePos);

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
  Toggle_Button(phost, CurProf.staggered, TAG_STAGGERED_TOGGLE, 92, 168, "On", "Off");

  Disp_End(phost);
}

void confirmAdvanceSetting(Gpu_Hal_Context_t *phost) {
  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);
  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));

  Gpu_CoCmd_Text(phost, 156, 35, 28, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "Proceed Advanced Setting?");
  App_WrCoCmd_Buffer(phost, TAG(246));
  Gpu_CoCmd_Button(phost, 179, 120, 60, 30, 28, 0, "No");
  App_WrCoCmd_Buffer(phost, TAG(247));
  Gpu_CoCmd_Button(phost, 68, 120, 60, 30, 28, 0, "Yes");

  Disp_End(phost);
}

#endif /* CONFIG_SCREEN_H */
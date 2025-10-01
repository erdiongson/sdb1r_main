#include "../gpu/App_Common.h"

void Config_Screen(Gpu_Hal_Context_t *phost) {

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
  App_WrCoCmd_Buffer(phost, TAG(13));
  App_WrCoCmd_Buffer(phost, VERTEX2F(160, 1008));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 704));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // No. of tube(row)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(14));
  App_WrCoCmd_Buffer(phost, VERTEX2F(1632, 1632));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2305, 1392));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // No. of tube(column)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(15));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2544, 1632));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3216, 1392));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // pitch(row)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(16));
  App_WrCoCmd_Buffer(phost, VERTEX2F(1632, 2032));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2305, 1776));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // pitch(column)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(17));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2544, 2032));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3216, 1776));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // origin(row)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(18));
  App_WrCoCmd_Buffer(phost, VERTEX2F(1632, 2432));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2305, 2160));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // origin(column)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(19));
  App_WrCoCmd_Buffer(phost, VERTEX2F(2544, 2432));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3216, 2160));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Cycles
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(20));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3744, 1632));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4480, 1392));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Z Dip
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(21));
  App_WrCoCmd_Buffer(phost, VERTEX2F(3744, 2248));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4480, 2008));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  App_WrCoCmd_Buffer(phost, END());

  //Text - Profile Name
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 150, 53, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, (const char *)CurProf.profileName);

  //20240918: erdiongson - Added for Password Enable and Disable process
  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_BgColor(phost, 0x00A2E8);
  // Check the password enable status and set the foreground color accordingly
  passwordStatus = CurProf.passwordEnabled ? 0 : 65535;
  if (passwordStatus == 0) {
    Gpu_CoCmd_FgColor(phost, 0x007300);               // Green when ON
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 128, 0));  //text color
  } else {
    Gpu_CoCmd_FgColor(phost, 0xFF0000);               // Red when OFF
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  //text color
  }

  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(PASSEN));
  Gpu_CoCmd_Toggle(phost, 242, 180, 40, 21, OPT_FLAT | OPT_FORMAT, passwordStatus, "On\xFFOff");
  Gpu_CoCmd_BgColor(phost, 0x00A2E8);
  Gpu_CoCmd_FgColor(phost, 0x007300);
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  //END

  //v204
  if (CurProf.Tube_No_x > MAXNUMX)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
  //v204
  //Text - Tube Number Row
  sprintf(buf, "%d", CurProf.Tube_No_x);
  Gpu_CoCmd_Text(phost, 122, 95, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  //v204
  //Text - Tube Number Column
  if (CurProf.Tube_No_y > MAXNUMY)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
  //v204
  sprintf(buf, "%d", CurProf.Tube_No_y);
  Gpu_CoCmd_Text(phost, 180, 95, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);
  //v204
  //Text - Pitch Row
  if (CurProf.Tube_No_x == 0) maxval = MAXXMM - CurProf.trayOriginX;
  else maxval = (MAXXMM - CurProf.trayOriginX) / (CurProf.Tube_No_x - 1);
  Round1Dec(&maxval);
  Dprint("max val Pitch X =", maxval);
  if (maxval < CurProf.pitch_x)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
                                                    //v204

  dtostrf(CurProf.pitch_x, 4, 1, buf);
  Gpu_CoCmd_Text(phost, 122, 119, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  //v204
  //Text - Pitch Column
  if (CurProf.Tube_No_y == 0) maxval = MAXYMM - CurProf.trayOriginY;
  else maxval = (MAXYMM - CurProf.trayOriginY) / (CurProf.Tube_No_y - 1);
  Round1Dec(&maxval);
  Dprint("max val Pitch Y =", maxval);
  if (maxval < CurProf.pitch_y)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
                                                    //v204

  dtostrf(CurProf.pitch_y, 4, 1, buf);
  Gpu_CoCmd_Text(phost, 180, 119, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  //v204
  //Text - Origin X
  if (CurProf.Tube_No_x == 0) maxval = MAXXMM;
  else maxval = MAXXMM - (CurProf.pitch_x * (CurProf.Tube_No_x - 1));
  Round1Dec(&maxval);
  Dprint("max val Org X =", maxval);
  if (maxval < CurProf.trayOriginX)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
                                                    //v204

  dtostrf(CurProf.trayOriginX, 4, 1, buf);
  Gpu_CoCmd_Text(phost, 122, 143, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

  //v204
  //Text - Origin Y
  if (CurProf.Tube_No_y == 0) maxval = MAXYMM;
  else maxval = MAXYMM - (CurProf.pitch_y * (CurProf.Tube_No_y - 1));
  Round1Dec(&maxval);
  Dprint("max val Org Y =", maxval);
  if (maxval < CurProf.trayOriginY)
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // red color text
  else
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));  // Change the color back to black
                                                    //v204

  dtostrf(CurProf.trayOriginY, 4, 1, buf);
  Gpu_CoCmd_Text(phost, 180, 143, 21, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, buf);

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
  App_WrCoCmd_Buffer(phost, TAG(11));
  Gpu_CoCmd_Button(phost, 116, 205, 76, 26, 21, 0, "Load");

  // Save Button
  //App_WrCoCmd_Buffer(phost, TAG_MASK(disableButtons ? 0 : 1)); // Enable or disable based on the flag
  App_WrCoCmd_Buffer(phost, TAG(12));
  Gpu_CoCmd_Button(phost, 8, 205, 76, 26, 21, 0, "Save");



  App_WrCoCmd_Buffer(phost, TAG(CONFIGADVANCE));  //disable advanced button
  Gpu_CoCmd_Button(phost, 233, 205, 76, 26, 21, 0, "Advanced");

  //20240903: erdiongson - add the variation of vibration U1 - U4
  vibstatus = CurProf.vibrationEnabled;
  if (vibstatus == 1) {
    App_WrCoCmd_Buffer(phost, TAG(VIBLVL));  //U1 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "1");
  } else if (vibstatus == 2) {
    App_WrCoCmd_Buffer(phost, TAG(VIBLVL));  //U2 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "2");
  } else if (vibstatus == 3) {
    App_WrCoCmd_Buffer(phost, TAG(VIBLVL));  //U3 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "3");
  } else if (vibstatus == 4) {
    App_WrCoCmd_Buffer(phost, TAG(VIBLVL));  //U4 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "4");
  } else {
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
    Gpu_CoCmd_FgColor(phost, 0xA1A1A1);
    App_WrCoCmd_Buffer(phost, TAG(VIBLVL));  //U0 Button
    Gpu_CoCmd_Button(phost, 101, 175, 44, 20, 27, 0, "0");
  }

  //20241001: erdiongson - add the variation of Vibration Duration (1-5)s
  vibtime_status = CurProf.vibrationDuration;
  if (vibtime_status == 3) {
    App_WrCoCmd_Buffer(phost, TAG(VIBDURATION));  //3 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "3s");
  } else if (vibtime_status == 4) {
    App_WrCoCmd_Buffer(phost, TAG(VIBDURATION));  //4 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "4s");
  } else if (vibtime_status == 5) {
    App_WrCoCmd_Buffer(phost, TAG(VIBDURATION));  //5 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "5s");
  } else if (vibtime_status == 1) {
    App_WrCoCmd_Buffer(phost, TAG(VIBDURATION));  //1 Second Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "1s");
  } else {
    App_WrCoCmd_Buffer(phost, TAG(VIBDURATION));  //2 Seconds Vibration
    Gpu_CoCmd_Button(phost, 159, 175, 44, 20, 27, 0, "2s");
  }

  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  Disp_End(phost);
}

void Skip_Screen(Gpu_Hal_Context_t *phost) {
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
  Gpu_CoCmd_Text(phost, 10, 20, 21, 0, "Skipped Columns:");
  Gpu_CoCmd_Text(phost, 10, 74, 21, 0, "Skipped Rows:");
  Gpu_CoCmd_Text(phost, 10, 128, 21, 0, "Skipped Single Position:");

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, BEGIN(RECTS));

  // Skipped Columns (edit name)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(SKIP_COLUMNS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(160, 1008));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 624));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Skipped Rows (edit name)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(SKIP_ROWS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(160, 1888));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 1504));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  // Skipped Single Position (edit name)
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(SKIP_SINGLE_POS));
  App_WrCoCmd_Buffer(phost, VERTEX2F(160, 2768));
  App_WrCoCmd_Buffer(phost, VERTEX2F(4944, 2384));
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));

  App_WrCoCmd_Buffer(phost, END());

  // Text - Skip Columns
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 15, 44, 21, 0, (const char *)CurProf.skipCol);

  // Text - Skip Rowa
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 15, 99, 21, 0, (const char *)CurProf.skipRow);

  // Text - Skip Columns
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
  Gpu_CoCmd_Text(phost, 15, 154, 21, 0, (const char *)CurProf.skipSinglePos);

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  Gpu_CoCmd_FgColor(phost, 0x00A2E8);

  // Back Button
  //App_WrCoCmd_Buffer(phost, TAG_MASK(disableButtons ? 0 : 1)); // Enable or disable based on the flag
  App_WrCoCmd_Buffer(phost, TAG(ADVPROF_BACK));
  Gpu_CoCmd_Button(phost, 247, 196, 62, 26, 21, 0, "Back");

  // Save Button
  //App_WrCoCmd_Buffer(phost, TAG_MASK(disableButtons ? 0 : 1)); // Enable or disable based on the flag
  App_WrCoCmd_Buffer(phost, TAG(ADVPROF_SAVE));
  Gpu_CoCmd_Button(phost, 10, 196, 62, 26, 21, 0, "Save");

  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(ADVPROF_BACK));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 247, 196, 62, 26, 21, (keypressed == ADVPROF_BACK) ? OPT_FLAT : 0, "Back");

  Gpu_CoCmd_FgColor(phost, 0x00A2E8);
  App_WrCoCmd_Buffer(phost, TAG(ADVPROF_SAVE));
  App_WrCoCmd_Buffer(phost, CLEAR_COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_Button(phost, 10, 196, 62, 26, 21, (keypressed == ADVPROF_SAVE) ? OPT_FLAT : 0, "Save");

  Disp_End(phost);
}


void config_screen_key_handler(Gpu_Hal_Context_t *phost, uint8_t keypressed) {
  char buf[PROFILE_NAME_MAX_LEN];
  float maxval = 0;
  int i;
  bool error = FALSE;

  switch (keypressed) {
    case 6:  // Home button
      Serial.println("Button Pressed: HOME");
      //delay(500);
      //while( Gpu_Hal_Rd32(phost, REG_TOUCH_SCREEN_XY) != 0x80008000);
      //v204
      error = FALSE;
      if (CurProf.Tube_No_x == 0) maxval = MAXXMM - CurProf.trayOriginX;
      else maxval = (MAXXMM - CurProf.trayOriginX) / (CurProf.Tube_No_x - 1);
      Round1Dec(&maxval);
      if (CurProf.pitch_x > maxval) error = TRUE;
      else {
        if (CurProf.Tube_No_y == 0) maxval = MAXYMM - CurProf.trayOriginY;
        else maxval = (MAXYMM - CurProf.trayOriginY) / (CurProf.Tube_No_y - 1);
        Round1Dec(&maxval);
        if (CurProf.pitch_y > maxval) error = TRUE;
      }
      //v204

      if (error) {
        strcpy(buf, CurProf.profileName);
        sprintf(CurProf.profileName, "Error in entry");
        Config_Screen(phost);
        delay(3000);
        strcpy(CurProf.profileName, buf);
        Config_Screen(phost);
        delay(3000);
      }
      break;
    case 11:  // Load (Config Screen)
      Serial.println("Button Pressed: LOAD");

      if (strcmp(CurProf.profileName, "xqreset") == 0) {  //special mode to preload eeprom
        Dprint("write preset data to eeprom");
        if (CurProfNum == 0) {
          for (i = 1; i < MAX_PROFILES; i++) {
            sprintf(CurProf.profileName, "%s %d", "Profile", i + 1);
            WriteProfileEEPROM(i);  //try054 ,CurProf);
          }
        } else PreLoadEEPROM();
        sprintf(CurProf.profileName, "EEprom reseted");
        Config_Screen(phost);
        delay(3000);
        sprintf(CurProf.profileName, "Profile 1 ");
        Config_Screen(phost);
      }
      if (strcmp(CurProf.profileName, "xqver") == 0) {  //special mode to preload eeprom
        Dprint("show version");
        sprintf(CurProf.profileName, "version : %s", FWVER);
        Config_Screen(phost);
        delay(3000);
        sprintf(CurProf.profileName, "xqver ");
        Config_Screen(phost);
      }
      if (strcmp(CurProf.profileName, "xqhome") == 0) {  //special mode to preload eeprom
        Dprint("home");
        Homing();
        Config_Screen(phost);
      }
      if (strcmp(CurProf.profileName, "xqblank") == 0) {  //special mode to preload eeprom
        Dprint("blankeeprom");
        BlankEEPROM();
        sprintf(CurProf.profileName, "EEprom blank");
        Config_Screen(phost);
        delay(3000);
        sprintf(CurProf.profileName, "xqblank ");
        Config_Screen(phost);
        delay(3000);
      }
      if (strcmp(CurProf.profileName, "xqsize-s") == 0) {  //special mode to use Version S (small) of the XY Table
        Dprint("Change the size to small.");
        sprintf(CurProf.profileName, "Size Change : SMALL");
        CurProf.sizeFlag = 0;
        Config_Screen(phost);
        delay(3000);
        sprintf(CurProf.profileName, "xqsize-s ");
        Config_Screen(phost);
      }
      Profile_Menu(&host);
      Config_Screen(phost);
      break;
    case CONFIGADVANCE:  // Advanced button
      Serial.println("Button Pressed: ADVANCED");
      Skip_Screen(phost);
      WaitKeyRelease();
      delay(100);
      // TODO: Tell the mode to go to the "skip screen"
      // Skip_Settings(phost);

      //Tray_Screen(phost);
      //Dprint("returntray","\n");
      // Config_Screen(phost);
      break;
    case 12:
      Serial.println("Button Pressed: SAVE");
      Dprint("curprofnum=", CurProfNum);
      WriteCurIDEEPROM(CurProfNum);
      WriteProfileEEPROM(CurProfNum);  //try054 ,CurProf);
      strcpy(buf, CurProf.profileName);
      strcpy(CurProf.profileName, "Profile saved");
      Config_Screen(phost);
      delay(3000);
      strcpy(CurProf.profileName, buf);
      break;
    case 13:
      Serial.println("Button Pressed: PROFILE");
      Keyboard(phost, CurProf.profileName, "Enter Profile Name", FALSE);
      Config_Screen(phost);
      break;

    case 14:  //no. of Tubes row
      maxval = (int)((MAXXMM - CurProf.trayOriginX) / CurProf.pitch_x) + 1;
      if (maxval > MAXNUMX) maxval = MAXNUMX;

      Dprint("max val=", maxval);
      CurProf.Tube_No_x = Keypad(&host, CurProf.Tube_No_x, MINNUMX, MAXNUMX, FALSE);  //v204 maxval,FALSE);
      Config_Screen(phost);
      break;
    case 15:  //no. of Tubes col
      maxval = (int)((MAXYMM - CurProf.trayOriginY) / CurProf.pitch_y) + 1;

      if (maxval > MAXNUMY) maxval = MAXNUMY;
      Dprint("max val=", maxval);
      CurProf.Tube_No_y = Keypad(&host, CurProf.Tube_No_y, MINNUMY, MAXNUMY, FALSE);  //v204 maxval,FALSE);
      Config_Screen(phost);
      break;
    case 16:  //pitch row
      if (CurProf.Tube_No_x == 0) maxval = MAXXMM - CurProf.trayOriginX;
      else maxval = (MAXXMM - CurProf.trayOriginX) / (CurProf.Tube_No_x - 1);
      Round1Dec(&maxval);
      Dprint("max val=", maxval);
      if (maxval > MAXPITCHX) maxval = MAXPITCHX;
      CurProf.pitch_x = Keypad(&host, CurProf.pitch_x, MINPITCHX, MAXPITCHX, FALSE);  //v204 maxval,TRUE);
      Config_Screen(phost);
      break;
    case 17:  //pitch col
      if (CurProf.Tube_No_y == 0) maxval = MAXYMM - CurProf.trayOriginY;
      else maxval = (MAXYMM - CurProf.trayOriginY) / (CurProf.Tube_No_y - 1);
      Round1Dec(&maxval);
      if (maxval > MAXPITCHY) maxval = MAXPITCHY;
      Dprint("max val=", maxval);
      CurProf.pitch_y = Keypad(&host, CurProf.pitch_y, MINPITCHY, MAXPITCHY, FALSE);  //v204 maxval,TRUE);
      Config_Screen(phost);
      break;
    case 18:  //origin row
      if (CurProf.Tube_No_x == 0) maxval = MAXXMM;
      else maxval = MAXXMM - (CurProf.pitch_x * (CurProf.Tube_No_x - 1));
      Round1Dec(&maxval);
      if (maxval > MAXORGX) maxval = MAXORGX;
      Dprint("max val=", maxval);

      CurProf.trayOriginX = Keypad(&host, CurProf.trayOriginX, 0, MAXORGX, FALSE);  //v204 maxval,TRUE);
      Config_Screen(phost);
      break;
    case 19:  //origin col
      if (CurProf.Tube_No_y == 0) maxval = MAXYMM;
      else maxval = MAXYMM - (CurProf.pitch_y * (CurProf.Tube_No_y - 1));
      Round1Dec(&maxval);
      if (maxval > MAXORGY) maxval = MAXORGY;
      Dprint("max val=", maxval);
      CurProf.trayOriginY = Keypad(&host, CurProf.trayOriginY, 0, MAXORGY, FALSE);  //v204 maxval,TRUE);
      Config_Screen(phost);
      break;

    case 20:  //Number of Cycles
      CurProf.Cycles = Keypad(&host, CurProf.Cycles, MINCYCLE, MAXCYCLE, FALSE);
      Config_Screen(phost);
      break;
    case 21:  //Z Dip
      CurProf.ZDip = Keypad(&host, CurProf.ZDip, MINZDIP, MAXZDIP, FALSE);
      Config_Screen(phost);
      break;

    case 28:  //Vibration Level
      // Logic handled in SettingsMode
      // TODO: Ensure this is working

      Config_Screen(phost);
      break;
    case PASSEN:  //Password Enable
      //passwordEnable_on();
      CurProf.passwordEnabled = !CurProf.passwordEnabled;  // Toggle the state
      Config_Screen(phost);
      break;
    case VIBDURATION:  //Vibration Duration
      // Logic handled in SettingsMode
      // TODO: Ensure this is working

      Config_Screen(phost);
      break;
      //case 246: // No button for Advanced Setting
      // Config_Screen(phost);
      // break;

      //case 247: // Yes button for Advanced Setting
      // Tray_Screen(phost, CurProf);
      // break;
    default:
      break;
  }
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

void skip_screen_key_handler(Gpu_Hal_Context_t *phost, uint8_t keypressed) {
  char buf[ROW_COL_MAX_LEN];
  int i;
  bool error = FALSE;

  switch (keypressed) {
    case SKIP_COLUMNS:
      Serial.println("Button Pressed: SKIP COLUMNS");
      Keyboard(phost, CurProf.skipCol, "Enter columns to skip", FALSE);
      Skip_Screen(phost);
      break;

    case SKIP_ROWS:
      Serial.println("Button Pressed: SKIP ROWS");
      Keyboard(phost, CurProf.skipRow, "Enter rows to skip", FALSE);
      Skip_Screen(phost);
      break;

    case SKIP_SINGLE_POS:
      Serial.println("Button Pressed: SKIP SINGLE POSITION");
      Keyboard(phost, CurProf.skipSinglePos, "Enter positions to skip", FALSE);
      Skip_Screen(phost);
      break;

    case ADVPROF_BACK:  // Back button
      Serial.println("Button Pressed: BACK");
      break;

    case ADVPROF_SAVE:
      Serial.println("Button Pressed: SAVE");
      Dprint("curprofnum=", CurProfNum);
      WriteCurIDEEPROM(CurProfNum);
      WriteProfileEEPROM(CurProfNum);  //try054 ,CurProf);
      strcpy(buf, CurProf.profileName);
      strcpy(CurProf.profileName, "Profile saved");
      Config_Screen(phost);
      delay(3000);
      strcpy(CurProf.profileName, buf);
      break;
    default:
      break;
  }
}
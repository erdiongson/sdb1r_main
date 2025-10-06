#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include "../gpu/App_Common.h"
#include "ErrorMessages.h"

// Parameters for Home_Screen display.
struct HomeParams {
  uint16_t current_row;
  uint16_t current_column;
  uint16_t tubes_left;
  int error_code;
};

inline void SetMainMenuButton(uint8_t whichmenu) {
  //					 	 SETTING START PAUSE STOP
  bool_t act_but[][6] = {
    { 1, 1, 0, 0 },  //main menu
    { 0, 0, 1, 1 },  //running menu
    { 0, 1, 0, 1 },  //pause menu
    { 0, 0, 0, 1 },  //error1 menu
    { 0, 0, 0, 1 },  //error2 menu
  };
  int i;
  char but[4] = { SETTING, START, PAUSE, STOP };

  for (i = 0; i < 4; i++) {
    App_WrCoCmd_Buffer(phost, TAG(but[i]));

    if (act_but[whichmenu][i]) {
      App_WrCoCmd_Buffer(phost, COLOR_A(255));
      App_WrCoCmd_Buffer(phost, TAG_MASK(1));
    } else {
      App_WrCoCmd_Buffer(phost, COLOR_A(60));
      App_WrCoCmd_Buffer(phost, TAG_MASK(0));
    }

    switch (but[i]) {
      case START:
        Gpu_CoCmd_FgColor(phost, 0x006400);
        if (whichmenu == PAUSEMENU)
          Gpu_CoCmd_Button(phost, 25, 112, 90, 36, 28, 0, "RESUME");
        else
          Gpu_CoCmd_Button(phost, 25, 112, 90, 36, 28, 0, "START");
        break;
      case PAUSE:
        Gpu_CoCmd_FgColor(phost, 0xADAF3C);
        Gpu_CoCmd_Button(phost, 207, 112, 90, 36, 28, 0, "PAUSE");
        break;
      case STOP:
        Gpu_CoCmd_FgColor(phost, 0xAA0000);
        if (whichmenu == TXRXERROR) {
          Gpu_CoCmd_Button(phost, 25, 157, 273, 36, 28, 0, "Communication error");
        } else if (whichmenu == HOMEERROR) {
          Gpu_CoCmd_Button(phost, 25, 157, 273, 36, 28, 0, "Home error");
        } else Gpu_CoCmd_Button(phost, 25, 157, 273, 36, 28, 0, "STOP");
        break;
      case SETTING:
        Gpu_CoCmd_FgColor(phost, 0x00A2E8);
        Gpu_CoCmd_Button(phost, 208, 5, 90, 36, 28, OPT_FORMAT, "Settings");
    }
    App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  }
}

inline void Home_Screen(Gpu_Hal_Context_t *phost, uint8_t whichmenu, const HomeParams* params = nullptr) {

  int32_t filling_tube = 0;
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


  SetMainMenuButton(whichmenu);

  sprintf(buf, "Profile Name: %s", CurProf.profileName);
  Gpu_CoCmd_Text(phost, 25, 195, 20, OPT_FORMAT, buf);  //OPT_CENTER | OPT_RIGHTX |
  sprintf(buf, "No. of Cycles: %d", CurProf.Cycles);
  Gpu_CoCmd_Text(phost, 25, 208, 20, OPT_FORMAT, buf);

  filling_tube = CurProf.Tube_No_x * CurProf.Tube_No_y;

  sprintf(buf, "Filling tube: %d", filling_tube);
  Gpu_CoCmd_Text(phost, 25, 220, 20, OPT_FORMAT, buf);

  sprintf(buf, "Current Tube : R%2d C%2d", params ? params->current_row : 1, params ? params->current_column : 1);
  Gpu_CoCmd_Text(phost, 292, 208, 20, OPT_RIGHTX | OPT_FORMAT, buf);

  sprintf(buf, "Tube left : %3d", params ? params->tubes_left : 0);
  Gpu_CoCmd_Text(phost, 294, 220, 20, OPT_RIGHTX | OPT_FORMAT, buf);

  //INSERT ERROR MESSAGE
  if (params && params->error_code > 0) {
    Error_Message(phost, params->error_code);
  }
  Disp_End(phost);
}

#endif

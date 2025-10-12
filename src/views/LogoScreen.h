#pragma once

#include "../gpu/Platform.h"
#include "../gpu/App_Common.h"
#include "common/Dialogs.h"

// Draws the logo screen with optional error dialog.
// @param phost GPU context.
// @param dialog_code Optional error code to display dialog (0 for no error).
void drawLogoScreen(Gpu_Hal_Context_t* phost, int dialog_code) {
  Gpu_CoCmd_FlashFast(phost, 0);
  Gpu_CoCmd_Dlstart(phost);

  Gpu_Hal_WrCmd32(phost, CMD_INFLATE);
  Gpu_Hal_WrCmd32(phost, RAM_XQ_LOGO);
  Gpu_Hal_WrCmdBufFromFlash(phost, (uint8_t*)XQ_Logo_trans, sizeof(XQ_Logo_trans));

  App_WrCoCmd_Buffer(phost, CLEAR(1, 1, 1));
  App_WrCoCmd_Buffer(phost, END());
  App_WrCoCmd_Buffer(phost, BITMAP_HANDLE(0));
  Gpu_CoCmd_SetBitmap(phost, 0, RGB565, 100, 82);
  App_WrCoCmd_Buffer(phost, BEGIN(BITMAPS));
  App_WrCoCmd_Buffer(phost, PALETTE_SOURCE(0));
  App_WrCoCmd_Buffer(phost, VERTEX2F(1856, 1152));
  App_WrCoCmd_Buffer(phost, END());

  // Draw firmware version at bottom of screen
  App_WrCoCmd_Buffer(phost, COLOR_RGB(40, 40, 40));
  Gpu_CoCmd_Text(phost, 160, 220, 20, OPT_CENTER | OPT_RIGHTX | OPT_FORMAT, "v" FWVER);

  if (dialog_code != 0) {
    drawDialog(phost, dialog_code);
  }

  Disp_End(phost);
  Gpu_Hal_Sleep(800);
}
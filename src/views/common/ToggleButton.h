#pragma once

#include "../../gpu/App_Common.h"

// Renders a toggle button with customizable colors based on enabled state.
// @param phost The GPU HAL context.
// @param enabled The toggle state (true = ON/enabled, false = OFF/disabled).
// @param tag The tag ID for the toggle button.
// @param x The x-coordinate of the toggle button.
// @param y The y-coordinate of the toggle button.
// @param enabledLabel The label to display when enabled.
// @param disabledLabel The label to display when disabled.
void Toggle_Button(Gpu_Hal_Context_t* phost, bool enabled, uint8_t tag, int16_t x, int16_t y, const char* enabledLabel,
                   const char* disabledLabel) {
  char labels[50];
  snprintf(labels, sizeof(labels), "%s\xFF%s", enabledLabel, disabledLabel);

  int16_t toggleState = enabled ? 0 : 65535;

  App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 255, 255));
  Gpu_CoCmd_BgColor(phost, 0x00A2E8);

  // Check the enabled state and set the foreground color accordingly
  if (enabled) {
    Gpu_CoCmd_FgColor(phost, 0x007300);               // Green when ON
    App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 128, 0));  // text color
  } else {
    Gpu_CoCmd_FgColor(phost, 0xFF0000);               // Red when OFF
    App_WrCoCmd_Buffer(phost, COLOR_RGB(255, 0, 0));  // text color
  }

  App_WrCoCmd_Buffer(phost, TAG_MASK(1));
  App_WrCoCmd_Buffer(phost, TAG(tag));
  Gpu_CoCmd_Toggle(phost, x, y, 40, 21, OPT_FLAT | OPT_FORMAT, toggleState, labels);
  Gpu_CoCmd_BgColor(phost, 0x00A2E8);
  Gpu_CoCmd_FgColor(phost, 0x007300);
  App_WrCoCmd_Buffer(phost, TAG_MASK(0));
  App_WrCoCmd_Buffer(phost, COLOR_RGB(0, 0, 0));
}

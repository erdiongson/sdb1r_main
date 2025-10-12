#pragma once

#include "../../gpu/Platform.h"
#include "../../gpu/App_Common.h"
#include "../../Constants.h"

// Dialog parameters structure.
typedef struct {
  const char* title;
  const char* subtitle;
  const char* left_btn;
  const char* right_btn;
  uint8_t left_tag;
  uint8_t right_tag;
} DialogParams;

// Displays a dialog with a black box, white outline, title, subtitle, and optional buttons.
// @param phost GPU context.
// @param params DialogParams structure containing title, subtitle, button labels and tags.
void drawBaseDialog(Gpu_Hal_Context_t* phost, const DialogParams& params);

// Displays a dialog based on dialog code.
// @param phost GPU context.
// @param dialog_code The dialog code to display.
void drawDialog(Gpu_Hal_Context_t* phost, int dialog_code);

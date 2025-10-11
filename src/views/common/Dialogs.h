#ifndef DIALOGS_H
#define DIALOGS_H

#include "../../gpu/Platform.h"
#include "../../gpu/App_Common.h"

// Dialog parameters structure.
typedef struct {
  const char *title;
  const char *subtitle;
  const char *left_btn;
  const char *right_btn;
  uint8_t left_tag;
  uint8_t right_tag;
} DialogParams;

// Dialog codes
#define DIALOG_ERROR_IR_SENSOR 2
#define DIALOG_ERROR_MARKER_NOT_DETECTED 3
#define DIALOG_ERROR_LIMIT_SWITCH 4
#define DIALOG_ERROR_LIMIT_SWITCH_HOMING 5
#define DIALOG_ERROR_ACK_ERROR 6
#define DIALOG_PROFILE_SAVED 7
#define DIALOG_PROFILE_LOADED 8
#define DIALOG_ERROR_PASSWORD_MISMATCH 9
#define DIALOG_PASSWORD_CHANGED 10
#define DIALOG_ERROR_WRONG_PASSWORD 11

// Displays a dialog with a black box, white outline, title, subtitle, and optional buttons.
// @param phost GPU context.
// @param params DialogParams structure containing title, subtitle, button labels and tags.
void drawBaseDialog(Gpu_Hal_Context_t *phost, const DialogParams& params);

// Displays a dialog based on dialog code.
// @param phost GPU context.
// @param dialog_code The dialog code to display.
void drawDialog(Gpu_Hal_Context_t *phost, int dialog_code);

#endif /* DIALOGS_H */

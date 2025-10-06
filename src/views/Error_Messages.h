#ifndef ERROR_MESSAGES_H
#define ERROR_MESSAGES_H

#include "../gpu/Platform.h"
#include "../gpu/App_Common.h"

// Error message parameters structure.
typedef struct {
  const char *title;
  const char *subtitle;
  const char *leftBtn;
  const char *rightBtn;
  uint8_t leftTag;
  uint8_t rightTag;
} ErrorMessageParams;

// Error codes
#define ERROR_TEST 1
#define ERROR_IR_SENSOR 2
#define ERROR_MARKER_NOT_DETECTED 3
#define ERROR_LIMIT_SWITCH 4
#define ERROR_ACK_ERROR 5

// Displays an error/message dialog with a black box, white outline, title, subtitle, and optional buttons.
// @param phost GPU context.
// @param params ErrorMessageParams structure containing title, subtitle, button labels and tags.
void Base_Error_Message(Gpu_Hal_Context_t *phost, const ErrorMessageParams& params);

// Displays an error message based on error code.
// @param phost GPU context.
// @param error_code The error code to display.
void Error_Message(Gpu_Hal_Context_t *phost, int error_code);

#endif /* ERROR_MESSAGES_H */

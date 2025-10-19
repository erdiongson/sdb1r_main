#pragma once

#include <avr/pgmspace.h>

// Shared temporary buffer for all view functions to reduce stack usage
// Size is set to the maximum needed across all view functions (60 bytes)
// Saves approximately 50-100 bytes of stack memory
static char g_view_temp_buffer[60];

// Helper macro to copy PROGMEM strings to RAM buffer for GPU rendering.
// Usage: Gpu_CoCmd_Button(phost, x, y, w, h, font, opt, PROGMEM_STR(F("Button")));
// @param progmem_str A PROGMEM string (use F() macro or PROGMEM constant).
// @return Pointer to g_view_temp_buffer containing the copied string.
#define PROGMEM_STR(progmem_str) \
  (strcpy_P(g_view_temp_buffer, (const char*)(progmem_str)), g_view_temp_buffer)

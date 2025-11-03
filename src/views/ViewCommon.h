#pragma once

#include <avr/pgmspace.h>
#include "../Buffers.h"

// Helper macro to copy PROGMEM strings to RAM buffer for GPU rendering.
// Usage: Gpu_CoCmd_Button(phost, x, y, w, h, font, opt, PROGMEM_STR(F("Button")));
// @param progmem_str A PROGMEM string (use F() macro or PROGMEM constant).
// @return Pointer to g_view_temp_buffer containing the copied string.
#define PROGMEM_STR(progmem_str) \
  (strncpy_P(g_view_temp_buffer, (const char*)(progmem_str), sizeof(g_view_temp_buffer) - 1), \
   g_view_temp_buffer[sizeof(g_view_temp_buffer) - 1] = '\0', \
   g_view_temp_buffer)

// Concatenates two PROGMEM strings with a custom format pattern.
// Usage: PROGMEM_STR_CONCAT(F("v%s - %s"), FWVER, F("Ready"))
// @param format_str PROGMEM format string (use F() macro).
// @param str1 First PROGMEM string (use F() macro or PROGMEM constant).
// @param str2 Second PROGMEM string (use F() macro or PROGMEM constant).
// @return Pointer to g_view_temp_buffer containing the formatted string.
inline const char* progmem_str_concat(const char* format_str, const char* str1, const char* str2) {
  // Load both strings into g_view_temp_buffer sequentially
  // str1 at offset 0, str2 at offset 30 (each limited to 29 chars + null terminator)
  strncpy_P(g_view_temp_buffer, str1, 29);
  g_view_temp_buffer[29] = '\0';
  strncpy_P(g_view_temp_buffer + 30, str2, 29);
  g_view_temp_buffer[59] = '\0';
  
  // Format them back into the start of g_view_temp_buffer
  snprintf_P(g_view_temp_buffer, sizeof(g_view_temp_buffer), format_str, 
             g_view_temp_buffer, g_view_temp_buffer + 30);
  return g_view_temp_buffer;
}

#define PROGMEM_STR_CONCAT(format_str, str1, str2) \
  progmem_str_concat((const char*)(format_str), (const char*)(str1), (const char*)(str2))

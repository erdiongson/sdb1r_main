#include <Arduino.h>
#include "Buffers.h"

// Global buffer definitions - only one copy in memory
char g_log_buffer[100];
char g_intermediate_token_buffer[g_intermediate_token_buffer_size];
char g_intermediate_buffer[g_intermediate_buffer_size];
SkipPosition g_intermediate_skip_positions[MAX_SKIP_POSITIONS_TOTAL];
char g_error_buffer[g_error_buffer_size];
char g_view_temp_buffer[60];
char g_skip_col_buffer[SKIP_STRING_LEN];
char g_skip_row_buffer[SKIP_STRING_LEN];
char g_skip_single_buffer[SKIP_STRING_LEN];

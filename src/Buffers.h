#pragma once
#include "Constants.h"
#include "logic/Profile.h"

// Shared buffer for logging to avoid String concatenations and heap allocations
// Size is set to accommodate the longest log message in the codebase
extern char g_log_buffer[100];

// Shared buffer when needing to store intermediate data relating to token generation (e.g. C2, C1R8)
const int g_intermediate_token_buffer_size = 10;
extern char g_intermediate_token_buffer[g_intermediate_token_buffer_size];

// Shared buffer when needing to store intermediate data relating string manipulation and validation
const int g_intermediate_buffer_size = SKIP_STRING_LEN + 1;
extern char g_intermediate_buffer[g_intermediate_buffer_size];

// Shared buffer when needing to store intermediate data relating to skip positions
extern SkipPosition g_intermediate_skip_positions[MAX_SKIP_POSITIONS_TOTAL];

// Shared buffer when needing to store error messages
const int g_error_buffer_size = 100;
extern char g_error_buffer[g_error_buffer_size];

// Shared temporary buffer for all view functions to reduce stack usage
// Size is set to the maximum needed across all view functions (60 bytes)
extern char g_view_temp_buffer[60];

extern char g_skip_col_buffer[SKIP_STRING_LEN];
extern char g_skip_row_buffer[SKIP_STRING_LEN];
extern char g_skip_single_buffer[SKIP_STRING_LEN];
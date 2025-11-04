#pragma once
#include "Constants.h"
#include "logic/Profile.h"

// Shared buffer for logging to avoid String concatenations and heap allocations
// Size is set to accommodate the longest log message in the codebase
extern char g_log_buffer[100];

// Shared buffer when needing to store intermediate data relating to token generation (e.g. C2, C1R8)
// Should be used locally only to avoid conflicts with other functions
const int g_intermediate_token_buffer_size = 10;
extern char g_intermediate_token_buffer[g_intermediate_token_buffer_size];

// Shared buffer when needing to store intermediate data relating string manipulation and validation
// Should be used locally only to avoid conflicts with other functions
const int g_intermediate_buffer_size = SKIP_STRING_LEN + 1;
extern char g_intermediate_buffer[g_intermediate_buffer_size];

// Shared buffer when needing to store intermediate data relating to skip positions
// Should be used locally only to avoid conflicts with other functions
extern SkipPosition g_intermediate_skip_positions[MAX_SKIP_POSITIONS_TOTAL];

// Shared buffer when needing to store messages (e.g. error messages, status messages, etc)
// Typically used to pass message strings between functions / controller-views
const int g_message_buffer_size = 150;
extern char g_message_buffer[g_message_buffer_size];

// Shared buffer when needing to store short-lived data for views
// Should be used locally only to avoid conflicts with other functions
extern char g_view_temp_buffer[60];

// Shared buffer when needing to store skip position strings
// Used typically in skip position parsing, validation, and rendering
extern char g_skip_col_buffer[SKIP_STRING_LEN];
extern char g_skip_row_buffer[SKIP_STRING_LEN];
extern char g_skip_single_buffer[SKIP_STRING_LEN];
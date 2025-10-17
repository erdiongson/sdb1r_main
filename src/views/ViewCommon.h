#pragma once

// Shared temporary buffer for all view functions to reduce stack usage
// Size is set to the maximum needed across all view functions (60 bytes)
// Saves approximately 50-100 bytes of stack memory
static char g_view_temp_buffer[60];

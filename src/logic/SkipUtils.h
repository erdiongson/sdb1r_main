#pragma once

#include <Arduino.h>
#include "../logic/Profile.h"
#include "../Constants.h"
#include "TrayPositionHandler.h"
#include "../Utils.h"

// Forward declaration - Position must be defined before including this header
namespace TrayHandler {
  struct Position;
}

class SkipUtils {
 private:
  // Helper function to check if a string contains only digits.
  // @param str The string to check.
  // @return True if string contains only digits, false otherwise.
  static bool isAllDigits(const char* str) {
    if (str == nullptr || str[0] == '\0') return false;
    for (int i = 0; str[i] != '\0'; i++) {
      if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
  }

 public:
  // Helper function to check if a skip position is valid and within bounds.
  // @param pos The position to check.
  // @param dimensions The tray dimensions to validate against.
  // @param staggered If true, considers last column of even rows as invalid (staggered mode).
  // @return True if position is valid and within bounds, false otherwise.
  static bool isValidSkipPosition(const TrayHandler::Position& pos, const TrayHandler::Dimensions& dimensions, bool staggered = false) {
    // Invalid position marker
    if (pos.isInvalid()) return false;

    // Column skip: x should be within [1, dimensions.columns], y is 0
    if (pos.y == 0 && pos.x != 0) {
      return (pos.x >= 1 && pos.x <= dimensions.columns);
    }

    // Row skip: y should be within [1, dimensions.rows], x is 0
    if (pos.x == 0 && pos.y != 0) {
      return (pos.y >= 1 && pos.y <= dimensions.rows);
    }

    // Individual position skip: both x and y should be within bounds
    if (pos.x != 0 && pos.y != 0) {
      bool within_bounds = (pos.x >= 1 && pos.x <= dimensions.columns && pos.y >= 1 && pos.y <= dimensions.rows);
      
      if (!within_bounds) return false;
      
      // If staggered mode is enabled, last column of every even row is invalid
      if (staggered && pos.y % 2 == 0 && pos.x == dimensions.columns) {
        return false;
      }
      
      return true;
    }

    return false;
  }

  enum SkipType { ROW, COLUMN, INDIVIDUAL };

  // Converts SkipPosition array to char string arrays (for UI display).
  // @param skip_positions Array of SkipPosition from EEPROM.
  // @param count Number of positions in the array.
  // @param out_skip_col Output buffer for column skip string (format: "C1,C9,...").
  // @param out_skip_col_size Size of out_skip_col buffer.
  // @param out_skip_row Output buffer for row skip string (format: "R1,R9,...").
  // @param out_skip_row_size Size of out_skip_row buffer.
  // @param out_skip_single_pos Output buffer for individual position skip string (format: "C2R4,C3R4,...").
  // @param out_skip_single_pos_size Size of out_skip_single_pos buffer.
  static void convertToStrings(const SkipPosition* skip_positions, uint8_t count,
                                char* out_skip_col, int out_skip_col_size,
                                char* out_skip_row, int out_skip_row_size,
                                char* out_skip_single_pos, int out_skip_single_pos_size) {
    if (!skip_positions || count == 0) {
      out_skip_col[0] = '\0';
      out_skip_row[0] = '\0';
      out_skip_single_pos[0] = '\0';
      return;
    };
    if (!out_skip_col || !out_skip_row || !out_skip_single_pos) return;

    out_skip_col[0] = '\0';
    out_skip_row[0] = '\0';
    out_skip_single_pos[0] = '\0';

    int col_len = 0;
    int row_len = 0;
    int single_len = 0;

    for (uint8_t i = 0; i < count && i < MAX_SKIP_POSITIONS_TOTAL; i++) {
      const SkipPosition& pos = skip_positions[i];

      // Column skip: x != 0, y == 0
      if (pos.x != 0 && pos.y == 0) {
        int needed = snprintf(g_intermediate_token_buffer, g_intermediate_token_buffer_size, "%sC%d", col_len > 0 ? "," : "", pos.x);
        if (col_len + needed < out_skip_col_size) {
          strcat(out_skip_col, g_intermediate_token_buffer);
          col_len += needed;
        }
      }
      // Row skip: x == 0, y != 0
      else if (pos.x == 0 && pos.y != 0) {
        int needed = snprintf(g_intermediate_token_buffer, g_intermediate_token_buffer_size, "%sR%d", row_len > 0 ? "," : "", pos.y);
        if (row_len + needed < out_skip_row_size) {
          strcat(out_skip_row, g_intermediate_token_buffer);
          row_len += needed;
        }
      }
      // Individual position skip: x != 0, y != 0
      else if (pos.x != 0 && pos.y != 0) {
        int needed = snprintf(g_intermediate_token_buffer, g_intermediate_token_buffer_size, "%sC%dR%d", single_len > 0 ? "," : "", pos.x, pos.y);
        if (single_len + needed < out_skip_single_pos_size) {
          strcat(out_skip_single_pos, g_intermediate_token_buffer);
          single_len += needed;
        }
      }
    }
  }

  // Converts char string arrays to SkipPosition array (for EEPROM storage).
  // @param skip_col Column skip string (format: "C1,C9,...").
  // @param skip_row Row skip string (format: "R1,R9,...").
  // @param skip_single_pos Individual position skip string (format: "C2R4,C3R4,...").
  // @param out_positions Output array to store SkipPosition objects.
  // @param max_positions Maximum number of positions to store.
  // @return The number of positions stored.
  static uint8_t convertFromStrings(const char* skip_col, const char* skip_row, const char* skip_single_pos,
                                     SkipPosition* out_positions, uint8_t max_positions) {
    if (!out_positions || max_positions == 0) return 0;

    uint8_t pos_index = 0;

    // Parse skip_col string (format: "C1,C9,...")
    if (skip_col && skip_col[0] != '\0') {
      int i = 0;
      while (skip_col[i] != '\0') {
        if (skip_col[i] == ',') {
          i++;
          continue;
        }
        
        if (skip_col[i] == 'C') {
          int col = atoi(skip_col + i + 1);
          if (col > 0 && col <= 255) {
            if (pos_index < max_positions) { out_positions[pos_index] = SkipPosition(col, 0); }
            pos_index++;
          }
        }
        
        // Skip to next comma or end
        while (skip_col[i] != '\0' && skip_col[i] != ',') { i++; }
      }
    }

    // Parse skip_row string (format: "R1,R9,...")
    if (skip_row && skip_row[0] != '\0') {
      int i = 0;
      while (skip_row[i] != '\0') {
        if (skip_row[i] == ',') {
          i++;
          continue;
        }
        
        if (skip_row[i] == 'R') {
          int row = atoi(skip_row + i + 1);
          if (row > 0 && row <= 255) {
            if (pos_index < max_positions) { out_positions[pos_index] = SkipPosition(0, row); }
            pos_index++;
          }
        }
        
        // Skip to next comma or end
        while (skip_row[i] != '\0' && skip_row[i] != ',') { i++; }
      }
    }

    // Parse skip_single_pos string (format: "C2R4,C3R4,...")
    if (skip_single_pos && skip_single_pos[0] != '\0') {
      int i = 0;
      while (skip_single_pos[i] != '\0') {
        if (skip_single_pos[i] == ',') {
          i++;
          continue;
        }
        
        // Find token boundaries
        int token_start = i;
        while (skip_single_pos[i] != '\0' && skip_single_pos[i] != ',') { i++; }
        
        // Parse CXRX format
        if (skip_single_pos[token_start] == 'C') {
          // Find R position within token
          int r_pos = -1;
          for (int j = token_start + 1; j < i; j++) {
            if (skip_single_pos[j] == 'R') {
              r_pos = j;
              break;
            }
          }
          
          if (r_pos > token_start + 1) {
            int col = atoi(skip_single_pos + token_start + 1);
            int row = atoi(skip_single_pos + r_pos + 1);
            
            if (col > 0 && col <= 255 && row > 0 && row <= 255) {
              if (pos_index < max_positions) { out_positions[pos_index] = SkipPosition(col, row); }
              pos_index++;
            }
          }
        }
      }
    }

    return pos_index;
  }

  // Converts profile skip data into an array of Position objects for TrayHandler.
  // @param profile The profile containing skip information.
  // @param out_positions Output array to store parsed positions (must be at least MAX_SKIP_POSITIONS_TOTAL in size).
  // @return The number of positions parsed.
  static int convert(Profile& profile, TrayHandler::Position out_positions[MAX_SKIP_POSITIONS_TOTAL]) {
    // Initialize all positions to 255 to mark unused entries
    for (int i = 0; i < MAX_SKIP_POSITIONS_TOTAL; i++) {
      out_positions[i] = TrayHandler::Position(255, 255);
    }

    // Convert from SkipPosition array directly
    int pos_index = 0;
    for (uint8_t i = 0; i < profile.skip_count && i < MAX_SKIP_POSITIONS_TOTAL && pos_index < MAX_SKIP_POSITIONS_TOTAL; i++) {
      const SkipPosition& skip_pos = profile.skip_positions[i];
      out_positions[pos_index++] = TrayHandler::Position(skip_pos.x, skip_pos.y);
    }

    return pos_index;
  }

  // Converts a single skip element string to a Position.
  // @param element The element string (e.g., "C1", "R5", "C2R4").
  // @param type The type of skip string (ROW, COLUMN, or INDIVIDUAL).
  // @return Position object. Returns Position(255, 255) if invalid.
  static TrayHandler::Position convertElement(const char* element, SkipType type) {
    if (element == nullptr || element[0] == '\0') {
      return TrayHandler::Position(255, 255);
    }

    if (type == COLUMN) {
      // Format: C<number>
      if (element[0] == 'C' && strlen(element) > 1 && isAllDigits(element + 1)) {
        int col = atoi(element + 1);
        if (col > 0) {
          return TrayHandler::Position(col, 0);
        }
      }
    } else if (type == ROW) {
      // Format: R<number>
      if (element[0] == 'R' && strlen(element) > 1 && isAllDigits(element + 1)) {
        int row = atoi(element + 1);
        if (row > 0) {
          return TrayHandler::Position(0, row);
        }
      }
    } else if (type == INDIVIDUAL) {
      // Format: C<number>R<number> (max 3 digits per number)
      if (element[0] == 'C') {
        char* row_str = strchr(element + 1, 'R');
        if (row_str != nullptr) {
          int col_num_len = row_str - (element + 1);
          // Max 3 digits per number, so col_num_len should be <= 3
          if (col_num_len > 0 && col_num_len <= 3) {
            char col_num[4];  // Max 3 digits + null terminator
            strncpy(col_num, element + 1, col_num_len);
            col_num[col_num_len] = '\0';

            if (isAllDigits(col_num) && isAllDigits(row_str + 1)) {
              int col = atoi(col_num);
              int row = atoi(row_str + 1);

              if (col > 0 && row > 0) {
                return TrayHandler::Position(col, row);
              }
            }
          }
        }
      }
    }

    return TrayHandler::Position(255, 255);
  }

  struct FormatParams {
    char* buffer;
    int buffer_size;
    char* error_buffer;
    int error_buffer_size;
    SkipType type;

    FormatParams(char* buffer, int buffer_size, SkipType type, char* error_buffer, int error_buffer_size)
      : buffer(buffer), buffer_size(buffer_size), error_buffer(error_buffer), error_buffer_size(error_buffer_size), type(type) {}
  };

  // Formats a skip string in-place by removing invalid characters and malformed entries.
  // @param params The FormatParams containing buffer, type, and error_message.
  static void format(const FormatParams& params) {
    if (params.buffer == nullptr) { return; }
    if (params.error_buffer != nullptr) params.error_buffer[0] = '\0';

    // Step 1: Capitalize 'c' to 'C' and 'r' to 'R' in-place
    for (int i = 0; i < params.buffer_size && params.buffer[i] != '\0'; i++) {
      if (params.buffer[i] == 'c') params.buffer[i] = 'C';
      if (params.buffer[i] == 'r') params.buffer[i] = 'R';
    }

    // Step 2: Check for invalid characters
    for (int i = 0; i < params.buffer_size && params.buffer[i] != '\0'; i++) {
      char c = params.buffer[i];

      // Check if character is valid based on type
      bool is_valid_char = false;
      if (params.type == COLUMN) {
        // Valid characters: C, digits, comma
        is_valid_char = (c == 'C' || (c >= '0' && c <= '9') || c == ',');
      } else if (params.type == ROW) {
        // Valid characters: R, digits, comma
        is_valid_char = (c == 'R' || (c >= '0' && c <= '9') || c == ',');
      } else if (params.type == INDIVIDUAL) {
        // Valid characters: C, R, digits, comma
        is_valid_char = (c == 'C' || c == 'R' || (c >= '0' && c <= '9') || c == ',');
      }

      if (!is_valid_char) {
        // Return on the first invalid character with an error message
        snprintf(params.error_buffer, params.error_buffer_size, "Error: Invalid character \"%c\" detected.", c);
        return;
      }
    }

    // Step 3: Parse and validate format (CXX, RXX, or CXRX where X is a digit)
    int i = 0;
    while (i < params.buffer_size && params.buffer[i] != '\0') {
      // Skip leading commas
      if (params.buffer[i] == ',') {
        i++;
        continue;
      }
      
      // Find end of current token
      int token_start = i;
      while (i < params.buffer_size && params.buffer[i] != '\0' && params.buffer[i] != ',') { i++; }
      int token_len = i - token_start;
      if (token_len == 0) continue;
      
      // Validate token format
      bool is_valid = false;
      
      if (params.type == COLUMN) {
        // Format: C followed by digits
        if (params.buffer[token_start] == 'C' && token_len > 1) {
          is_valid = true;
          for (int j = token_start + 1; j < token_start + token_len; j++) {
            if (params.buffer[j] < '0' || params.buffer[j] > '9') {
              is_valid = false;
              break;
            }
          }
        }
      } else if (params.type == ROW) {
        // Format: R followed by digits
        if (params.buffer[token_start] == 'R' && token_len > 1) {
          is_valid = true;
          for (int j = token_start + 1; j < token_start + token_len; j++) {
            if (params.buffer[j] < '0' || params.buffer[j] > '9') {
              is_valid = false;
              break;
            }
          }
        }
      } else if (params.type == INDIVIDUAL) {
        // Format: CXRX
        if (params.buffer[token_start] == 'C') {
          // Find R position
          int r_pos = -1;
          for (int j = token_start + 1; j < token_start + token_len; j++) {
            if (params.buffer[j] == 'R') {
              r_pos = j;
              break;
            }
          }
          
          if (r_pos > token_start + 1 && r_pos < token_start + token_len - 1) {
            is_valid = true;
            // Check digits before R
            for (int j = token_start + 1; j < r_pos; j++) {
              if (params.buffer[j] < '0' || params.buffer[j] > '9') {
                is_valid = false;
                break;
              }
            }
            // Check digits after R
            if (is_valid) {
              for (int j = r_pos + 1; j < token_start + token_len; j++) {
                if (params.buffer[j] < '0' || params.buffer[j] > '9') {
                  is_valid = false;
                  break;
                }
              }
            }
          }
        }
      }
      
      // Return on the first invalid token with an error message
      if (!is_valid) {
        int copy_len = token_len < sizeof(g_intermediate_token_buffer) - 1 ? token_len : sizeof(g_intermediate_token_buffer) - 1;
        strncpy(g_intermediate_token_buffer, params.buffer + token_start, copy_len);
        g_intermediate_token_buffer[copy_len] = '\0';
        snprintf(params.error_buffer, params.error_buffer_size, "Error: Invalid format \"%s\" detected.", g_intermediate_token_buffer);
        return;
      }
    }
  }

  struct CleanParams {
    const char* buffer;
    int buffer_size;
    const char* error_buffer;
    int error_buffer_size;
    SkipType type;
    TrayHandler::Dimensions dimensions;
    bool staggered;

    CleanParams(const char* buffer, int buffer_size, const char* error_buffer, int error_buffer_size, SkipType type, const TrayHandler::Dimensions& dimensions, bool staggered = false)
      : buffer(buffer), buffer_size(buffer_size), error_buffer(error_buffer), error_buffer_size(error_buffer_size), type(type), dimensions(dimensions), staggered(staggered) {}
  };


  struct CleanResult {
    bool is_valid;
    CleanResult() : is_valid(false) { }
  };

  // Cleans and validates a skip string, checking bounds against dimensions.
  // @param params The CleanParams containing input string, type, dimensions, and staggered flag.
  // @return CleanResult containing the cleaned string, error message (if any), and whether it was modified.
  static CleanResult clean(const CleanParams& params) {
    CleanResult result;
    result.is_valid = false;
    int max_skip_positions = params.type == INDIVIDUAL ? MAX_SKIP_POSITIONS_INDIVIDUAL : params.type == ROW ? MAX_SKIP_POSITIONS_ROWS : MAX_SKIP_POSITIONS_COLUMNS;

    if (params.buffer == nullptr) { return result; }

    // Step 1: Format the string (remove invalid characters, validate format)
    format(FormatParams(const_cast<char*>(params.buffer), params.buffer_size, params.type, params.error_buffer, params.error_buffer_size));

    // If there was a format error, return it with the formatted (capitalized) string
    if (params.error_buffer[0] != '\0') { return result; }

    // Step 2: Parse formatted string and check bounds
    int total_count = 0;

    // Parse input string manually
    int i = 0;
    while (i < params.buffer_size && params.buffer[i] != '\0') {
      // Skip leading commas
      if (params.buffer[i] == ',') {
        i++;
        continue;
      }
      
      // Find token boundaries
      int token_start = i;
      while (i < params.buffer_size && params.buffer[i] != '\0' && params.buffer[i] != ',') { i++; }
      int token_len = i - token_start;
      
      if (token_len == 0) continue;
      
      // Parse position based on type
      TrayHandler::Position pos(255, 255);
      
      if (params.type == COLUMN && params.buffer[token_start] == 'C') {
        int col = atoi(params.buffer + token_start + 1);
        if (col > 0) pos = TrayHandler::Position(col, 0);
      } else if (params.type == ROW && params.buffer[token_start] == 'R') {
        int row = atoi(params.buffer + token_start + 1);
        if (row > 0) pos = TrayHandler::Position(0, row);
      } else if (params.type == INDIVIDUAL && params.buffer[token_start] == 'C') {
        // Find R position within token
        int r_pos = -1;
        for (int j = token_start + 1; j < token_start + token_len; j++) {
          if (params.buffer[j] == 'R') {
            r_pos = j;
            break;
          }
        }
        if (r_pos > token_start + 1) {
          int col = atoi(params.buffer + token_start + 1);
          int row = atoi(params.buffer + r_pos + 1);
          if (col > 0 && row > 0) pos = TrayHandler::Position(col, row);
        }
      }
      
      // Check if position is valid and within bounds
      if (!isValidSkipPosition(pos, params.dimensions, params.staggered)) {
        // Return immediately on first out-of-bounds position
        int copy_len = token_len < sizeof(g_intermediate_token_buffer) - 1 ? token_len : sizeof(g_intermediate_token_buffer) - 1;
        strncpy(g_intermediate_token_buffer, params.buffer + token_start, copy_len);
        g_intermediate_token_buffer[copy_len] = '\0';
        snprintf(params.error_buffer, params.error_buffer_size, "Position \"%s\" is out of bounds.", g_intermediate_token_buffer);
        return result;
      }
      
      // Check for duplicate by comparing with seen positions
      SkipPosition skip_pos(pos.x, pos.y);
      for (int j = 0; j < total_count; j++) {
        if (g_intermediate_skip_positions[j].x == skip_pos.x && g_intermediate_skip_positions[j].y == skip_pos.y) {
          int copy_len = token_len < sizeof(g_intermediate_token_buffer) - 1 ? token_len : sizeof(g_intermediate_token_buffer) - 1;
          strncpy(g_intermediate_token_buffer, params.buffer + token_start, copy_len);
          g_intermediate_token_buffer[copy_len] = '\0';
          snprintf(params.error_buffer, params.error_buffer_size, "Duplicate position: %s", g_intermediate_token_buffer);
          return result;
        }
      }
      
      // Add to seen, unique positions for future duplicate checking
      if (total_count < MAX_SKIP_POSITIONS_TOTAL) {
        g_intermediate_skip_positions[total_count++] = skip_pos;
      }

      // Check if we have exceeded the maximum skip positions for the type
      if (total_count > max_skip_positions) {
        const char* type_name = (params.type == ROW) ? "row" : (params.type == COLUMN) ? "column" : "individual";
        snprintf(params.error_buffer, params.error_buffer_size, "Exceeded %d %s skip positions.", max_skip_positions, type_name);
        return result;
      }
    }

    // If we get here, the input is valid
    result.is_valid = true;
    return result;
  }
};
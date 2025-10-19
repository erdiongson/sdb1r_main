#pragma once

#include <Arduino.h>
#include "../logic/Profile.h"
#include "../Constants.h"

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
    if (pos.x == -1 && pos.y == -1) return false;

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

  struct CleanResult {
    char cleaned[SKIP_STRING_LEN];
    bool was_cleaned;
    char error_message[SKIP_STRING_LEN];

    CleanResult() : was_cleaned(false) { 
      cleaned[0] = '\0'; 
      error_message[0] = '\0';
    }
  };

  // Converts SkipPosition array to char string arrays (for UI display).
  // @param skip_positions Array of SkipPosition from EEPROM.
  // @param count Number of positions in the array.
  // @param out_skip_col Output buffer for column skip string (format: "C1,C9,...").
  // @param out_skip_row Output buffer for row skip string (format: "R1,R9,...").
  // @param out_skip_single_pos Output buffer for individual position skip string (format: "C2R4,C3R4,...").
  static void convertToStrings(const SkipPosition* skip_positions, uint8_t count,
                                char* out_skip_col, char* out_skip_row, char* out_skip_single_pos) {
    if (out_skip_col) out_skip_col[0] = '\0';
    if (out_skip_row) out_skip_row[0] = '\0';
    if (out_skip_single_pos) out_skip_single_pos[0] = '\0';

    if (!skip_positions || count == 0) return;

    bool first_col = true, first_row = true, first_pos = true;
    char temp[20];

    for (uint8_t i = 0; i < count && i < MAX_SKIP_POSITIONS; i++) {
      const SkipPosition& pos = skip_positions[i];

      // Column skip: x != 0, y == 0
      if (pos.x != 0 && pos.y == 0) {
        if (!first_col && out_skip_col) strcat(out_skip_col, ",");
        sprintf(temp, "C%d", pos.x);
        if (out_skip_col) strcat(out_skip_col, temp);
        first_col = false;
      }
      // Row skip: x == 0, y != 0
      else if (pos.x == 0 && pos.y != 0) {
        if (!first_row && out_skip_row) strcat(out_skip_row, ",");
        sprintf(temp, "R%d", pos.y);
        if (out_skip_row) strcat(out_skip_row, temp);
        first_row = false;
      }
      // Individual position skip: x != 0, y != 0
      else if (pos.x != 0 && pos.y != 0) {
        if (!first_pos && out_skip_single_pos) strcat(out_skip_single_pos, ",");
        sprintf(temp, "C%dR%d", pos.x, pos.y);
        if (out_skip_single_pos) strcat(out_skip_single_pos, temp);
        first_pos = false;
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
      char temp_col[SKIP_STRING_LEN];
      strncpy(temp_col, skip_col, SKIP_STRING_LEN - 1);
      temp_col[SKIP_STRING_LEN - 1] = '\0';

      char* token = strtok(temp_col, ",");
      while (token != nullptr && pos_index < max_positions) {
        if (token[0] == 'C') {
          int col = atoi(token + 1);
          if (col > 0 && col <= 255) {
            out_positions[pos_index++] = SkipPosition(col, 0);
          }
        }
        token = strtok(nullptr, ",");
      }
    }

    // Parse skip_row string (format: "R1,R9,...")
    if (skip_row && skip_row[0] != '\0') {
      char temp_row[SKIP_STRING_LEN];
      strncpy(temp_row, skip_row, SKIP_STRING_LEN - 1);
      temp_row[SKIP_STRING_LEN - 1] = '\0';

      char* token = strtok(temp_row, ",");
      while (token != nullptr && pos_index < max_positions) {
        if (token[0] == 'R') {
          int row = atoi(token + 1);
          if (row > 0 && row <= 255) {
            out_positions[pos_index++] = SkipPosition(0, row);
          }
        }
        token = strtok(nullptr, ",");
      }
    }

    // Parse skip_single_pos string (format: "C2R4,C3R4,...")
    if (skip_single_pos && skip_single_pos[0] != '\0') {
      char temp_pos[SKIP_STRING_LEN];
      strncpy(temp_pos, skip_single_pos, SKIP_STRING_LEN - 1);
      temp_pos[SKIP_STRING_LEN - 1] = '\0';

      char* token = strtok(temp_pos, ",");
      while (token != nullptr && pos_index < max_positions) {
        int col = 0, row = 0;
        char* col_str = strstr(token, "C");
        char* row_str = strstr(token, "R");

        if (col_str && row_str) {
          col = atoi(col_str + 1);
          row = atoi(row_str + 1);

          if (col > 0 && col <= 255 && row > 0 && row <= 255) {
            out_positions[pos_index++] = SkipPosition(col, row);
          }
        }
        token = strtok(nullptr, ",");
      }
    }

    return pos_index;
  }

  // Converts profile skip data into an array of Position objects for TrayHandler.
  // @param profile The profile containing skip information.
  // @param out_positions Output array to store parsed positions (must be at least MAX_SKIP_POSITIONS in size).
  // @return The number of positions parsed.
  static int convert(Profile& profile, TrayHandler::Position out_positions[MAX_SKIP_POSITIONS]) {
    // Initialize all positions to -1 to mark unused entries
    for (int i = 0; i < MAX_SKIP_POSITIONS; i++) {
      out_positions[i] = TrayHandler::Position(-1, -1);
    }

    // Convert from SkipPosition array directly
    int pos_index = 0;
    for (uint8_t i = 0; i < profile.skip_count && i < MAX_SKIP_POSITIONS && pos_index < MAX_SKIP_POSITIONS; i++) {
      const SkipPosition& skip_pos = profile.skip_positions[i];
      out_positions[pos_index++] = TrayHandler::Position(skip_pos.x, skip_pos.y);
    }

    return pos_index;
  }

  // Converts a single skip element string to a Position.
  // @param element The element string (e.g., "C1", "R5", "C2R4").
  // @param type The type of skip string (ROW, COLUMN, or INDIVIDUAL).
  // @return Position object. Returns Position(-1, -1) if invalid.
  static TrayHandler::Position convertElement(const char* element, SkipType type) {
    if (element == nullptr || element[0] == '\0') {
      return TrayHandler::Position(-1, -1);
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
      // Format: C<number>R<number>
      if (element[0] == 'C') {
        char* row_str = strchr(element + 1, 'R');
        if (row_str != nullptr) {
          int col_num_len = row_str - (element + 1);
          if (col_num_len > 0) {
            char col_num[SKIP_STRING_LEN];
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

    return TrayHandler::Position(-1, -1);
  }

  // Formats a skip string by removing invalid characters and malformed entries.
  // @param input The input string to format.
  // @param type The type of skip string (ROW, COLUMN, or INDIVIDUAL).
  // @param output Buffer to store the formatted string (must be at least SKIP_STRING_LEN in size).
  // @param error_message Buffer to store error message if invalid characters or format detected (must be at least SKIP_STRING_LEN in size).
  static void format(const char* input, SkipType type, char* output, char* error_message) {
    if (input == nullptr || output == nullptr) {
      if (output != nullptr) output[0] = '\0';
      if (error_message != nullptr) error_message[0] = '\0';
      return;
    }

    // Initialize output and error message
    output[0] = '\0';
    if (error_message != nullptr) error_message[0] = '\0';

    // Step 1: Capitalize 'c' to 'C' and 'r' to 'R'
    char capitalized[SKIP_STRING_LEN];
    int capitalized_idx = 0;

    for (int i = 0; input[i] != '\0' && capitalized_idx < SKIP_STRING_LEN - 1; i++) {
      char c = input[i];

      // Capitalize c to C and r to R
      if (c == 'c') c = 'C';
      if (c == 'r') c = 'R';

      capitalized[capitalized_idx++] = c;
    }
    capitalized[capitalized_idx] = '\0';

    strncpy(output, capitalized, SKIP_STRING_LEN - 1);
    output[SKIP_STRING_LEN - 1] = '\0';

    // Step 2: Remove all unnecessary characters
    char normalized[SKIP_STRING_LEN];
    int normalized_idx = 0;
    bool has_invalid_char = false;
    char first_invalid_char = '\0';

    for (int i = 0; capitalized[i] != '\0' && normalized_idx < SKIP_STRING_LEN - 1; i++) {
      char c = capitalized[i];

      // Keep only valid characters based on type
      bool is_valid_char = false;
      if (type == COLUMN) {
        // Valid characters: C, digits, comma
        is_valid_char = (c == 'C' || (c >= '0' && c <= '9') || c == ',');
      } else if (type == ROW) {
        // Valid characters: R, digits, comma
        is_valid_char = (c == 'R' || (c >= '0' && c <= '9') || c == ',');
      } else if (type == INDIVIDUAL) {
        // Valid characters: C, R, digits, comma
        is_valid_char = (c == 'C' || c == 'R' || (c >= '0' && c <= '9') || c == ',');
      }

      if (is_valid_char) {
        normalized[normalized_idx++] = c;
      } else if (!has_invalid_char) {
        // Record first invalid character for error message
        has_invalid_char = true;
        first_invalid_char = c;
      }
    }
    normalized[normalized_idx] = '\0';

    if (has_invalid_char && error_message != nullptr) {
      sprintf(error_message, "Error: Invalid character \"%c\" detected.", first_invalid_char);
      return;
    }

    if (normalized[0] == '\0') {
      return;
    }

    // Step 3: Parse and validate format (CXX, RXX, or CXRX where X is a digit)
    char result[SKIP_STRING_LEN];
    result[0] = '\0';
    bool first_entry = true;
    bool has_invalid_format = false;
    char first_invalid_token[SKIP_STRING_LEN];
    first_invalid_token[0] = '\0';

    char temp[SKIP_STRING_LEN];
    strncpy(temp, normalized, SKIP_STRING_LEN - 1);
    temp[SKIP_STRING_LEN - 1] = '\0';

    char* token = strtok(temp, ",");
    while (token != nullptr) {
      // Skip empty tokens
      if (strlen(token) == 0) {
        token = strtok(nullptr, ",");
        continue;
      }

      bool is_valid = false;

      if (type == COLUMN) {
        // Format: C followed by digits (CXX where X is a digit)
        if (token[0] == 'C' && strlen(token) > 1 && isAllDigits(token + 1)) {
          int num = atoi(token + 1);
          if (num > 0) {
            is_valid = true;
            if (!first_entry) strcat(result, ",");
            strcat(result, token);
            first_entry = false;
          }
        }
      } else if (type == ROW) {
        // Format: R followed by digits (RXX where X is a digit)
        if (token[0] == 'R' && strlen(token) > 1 && isAllDigits(token + 1)) {
          int num = atoi(token + 1);
          if (num > 0) {
            is_valid = true;
            if (!first_entry) strcat(result, ",");
            strcat(result, token);
            first_entry = false;
          }
        }
      } else if (type == INDIVIDUAL) {
        // Format: C followed by digits, then R followed by digits (CXRX where X is a digit)
        if (token[0] == 'C') {
          char* row_str = strchr(token + 1, 'R');
          if (row_str != nullptr) {
            // Extract column number between C and R
            int col_num_len = row_str - (token + 1);
            if (col_num_len > 0) {
              char col_num[SKIP_STRING_LEN];
              strncpy(col_num, token + 1, col_num_len);
              col_num[col_num_len] = '\0';

              // Verify both parts are all digits
              if (isAllDigits(col_num) && isAllDigits(row_str + 1)) {
                int col = atoi(col_num);
                int row = atoi(row_str + 1);

                if (col > 0 && row > 0) {
                  is_valid = true;
                  if (!first_entry) strcat(result, ",");
                  strcat(result, token);
                  first_entry = false;
                }
              }
            }
          }
        }
      }

      // Record first invalid token for error message
      if (!is_valid && !has_invalid_format) {
        has_invalid_format = true;
        strncpy(first_invalid_token, token, SKIP_STRING_LEN - 1);
        first_invalid_token[SKIP_STRING_LEN - 1] = '\0';
      }

      token = strtok(nullptr, ",");
    }

    // Set error message for invalid format if no invalid character was detected
    if (has_invalid_format && error_message != nullptr && error_message[0] == '\0') {
      sprintf(error_message, "Error: Invalid format \"%s\" detected.", first_invalid_token);
      return;
    }

    strncpy(output, result, SKIP_STRING_LEN - 1);
    output[SKIP_STRING_LEN - 1] = '\0';
  }

  // Cleans and validates a skip string, checking bounds against dimensions.
  // @param input The input string to clean.
  // @param type The type of skip string (ROW, COLUMN, or INDIVIDUAL).
  // @param dimensions The tray dimensions to validate against.
  // @param staggered If true, considers last column of even rows as invalid (staggered mode).
  // @return CleanResult containing the cleaned string, error message (if any), and whether it was modified.
  static CleanResult clean(const char* input, SkipType type, const TrayHandler::Dimensions& dimensions, bool staggered = false) {
    CleanResult result;

    if (input == nullptr) {
      return result;
    }

    // Step 1: Format the string (remove invalid characters, validate format)
    char formatted[SKIP_STRING_LEN];
    char format_error[SKIP_STRING_LEN];
    format(input, type, formatted, format_error);

    // As long as provided a formatted string, return it later
    if (formatted[0] != '\0') {
      strncpy(result.cleaned, formatted, SKIP_STRING_LEN - 1);
      result.cleaned[SKIP_STRING_LEN - 1] = '\0';
    }

    // If there was a format error, return it with the formatted (capitalized) string
    if (format_error[0] != '\0') {
      strncpy(result.error_message, format_error, SKIP_STRING_LEN - 1);
      result.error_message[SKIP_STRING_LEN - 1] = '\0';
      return result;
    }

    // Step 2: Parse formatted string and check bounds
    char temp[SKIP_STRING_LEN];
    strncpy(temp, formatted, SKIP_STRING_LEN - 1);
    temp[SKIP_STRING_LEN - 1] = '\0';

    char final_result[SKIP_STRING_LEN];
    final_result[0] = '\0';
    bool first_entry = true;
    bool has_out_of_bounds = false;
    char first_out_of_bounds_token[SKIP_STRING_LEN];
    first_out_of_bounds_token[0] = '\0';

    char* token = strtok(temp, ",");
    while (token != nullptr) {
      // Convert element to Position
      TrayHandler::Position pos = convertElement(token, type);

      // Check if position is valid and within bounds
      if (isValidSkipPosition(pos, dimensions, staggered)) {
        if (!first_entry) strcat(final_result, ",");
        strcat(final_result, token);
        first_entry = false;
      } else if (!has_out_of_bounds) {
        // Record first out-of-bounds token for error message
        has_out_of_bounds = true;
        strncpy(first_out_of_bounds_token, token, SKIP_STRING_LEN - 1);
        first_out_of_bounds_token[SKIP_STRING_LEN - 1] = '\0';
      }

      token = strtok(nullptr, ",");
    }

    // If there was an out-of-bounds error, return it with the cleaned string
    if (has_out_of_bounds) {
      sprintf(result.error_message, "Error: Position \"%s\" is out of bounds.", first_out_of_bounds_token);
      return result;
    }

    // Copy final result to output
    strncpy(result.cleaned, final_result, SKIP_STRING_LEN - 1);
    result.cleaned[SKIP_STRING_LEN - 1] = '\0';

    // Check if input was modified
    result.was_cleaned = (strcmp(input, result.cleaned) != 0);

    return result;
  }
};
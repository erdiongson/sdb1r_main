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
      bool withinBounds = (pos.x >= 1 && pos.x <= dimensions.columns && pos.y >= 1 && pos.y <= dimensions.rows);
      
      if (!withinBounds) return false;
      
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

    bool firstCol = true, firstRow = true, firstPos = true;
    char temp[20];

    for (uint8_t i = 0; i < count && i < MAX_SKIP_POSITIONS; i++) {
      const SkipPosition& pos = skip_positions[i];

      // Column skip: x != 0, y == 0
      if (pos.x != 0 && pos.y == 0) {
        if (!firstCol && out_skip_col) strcat(out_skip_col, ",");
        sprintf(temp, "C%d", pos.x);
        if (out_skip_col) strcat(out_skip_col, temp);
        firstCol = false;
      }
      // Row skip: x == 0, y != 0
      else if (pos.x == 0 && pos.y != 0) {
        if (!firstRow && out_skip_row) strcat(out_skip_row, ",");
        sprintf(temp, "R%d", pos.y);
        if (out_skip_row) strcat(out_skip_row, temp);
        firstRow = false;
      }
      // Individual position skip: x != 0, y != 0
      else if (pos.x != 0 && pos.y != 0) {
        if (!firstPos && out_skip_single_pos) strcat(out_skip_single_pos, ",");
        sprintf(temp, "C%dR%d", pos.x, pos.y);
        if (out_skip_single_pos) strcat(out_skip_single_pos, temp);
        firstPos = false;
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

    uint8_t posIndex = 0;

    // Parse skip_col string (format: "C1,C9,...")
    if (skip_col && skip_col[0] != '\0') {
      char tempCol[SKIP_STRING_LEN];
      strncpy(tempCol, skip_col, SKIP_STRING_LEN - 1);
      tempCol[SKIP_STRING_LEN - 1] = '\0';

      char* token = strtok(tempCol, ",");
      while (token != nullptr && posIndex < max_positions) {
        if (token[0] == 'C') {
          int col = atoi(token + 1);
          if (col > 0 && col <= 255) {
            out_positions[posIndex++] = SkipPosition(col, 0);
          }
        }
        token = strtok(nullptr, ",");
      }
    }

    // Parse skip_row string (format: "R1,R9,...")
    if (skip_row && skip_row[0] != '\0') {
      char tempRow[SKIP_STRING_LEN];
      strncpy(tempRow, skip_row, SKIP_STRING_LEN - 1);
      tempRow[SKIP_STRING_LEN - 1] = '\0';

      char* token = strtok(tempRow, ",");
      while (token != nullptr && posIndex < max_positions) {
        if (token[0] == 'R') {
          int row = atoi(token + 1);
          if (row > 0 && row <= 255) {
            out_positions[posIndex++] = SkipPosition(0, row);
          }
        }
        token = strtok(nullptr, ",");
      }
    }

    // Parse skip_single_pos string (format: "C2R4,C3R4,...")
    if (skip_single_pos && skip_single_pos[0] != '\0') {
      char tempPos[SKIP_STRING_LEN];
      strncpy(tempPos, skip_single_pos, SKIP_STRING_LEN - 1);
      tempPos[SKIP_STRING_LEN - 1] = '\0';

      char* token = strtok(tempPos, ",");
      while (token != nullptr && posIndex < max_positions) {
        int col = 0, row = 0;
        char* colStr = strstr(token, "C");
        char* rowStr = strstr(token, "R");

        if (colStr && rowStr) {
          col = atoi(colStr + 1);
          row = atoi(rowStr + 1);

          if (col > 0 && col <= 255 && row > 0 && row <= 255) {
            out_positions[posIndex++] = SkipPosition(col, row);
          }
        }
        token = strtok(nullptr, ",");
      }
    }

    return posIndex;
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
    int posIndex = 0;
    for (uint8_t i = 0; i < profile.skip_count && i < MAX_SKIP_POSITIONS && posIndex < MAX_SKIP_POSITIONS; i++) {
      const SkipPosition& skipPos = profile.skip_positions[i];
      out_positions[posIndex++] = TrayHandler::Position(skipPos.x, skipPos.y);
    }

    return posIndex;
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
        char* rowStr = strchr(element + 1, 'R');
        if (rowStr != nullptr) {
          int colNumLen = rowStr - (element + 1);
          if (colNumLen > 0) {
            char colNum[SKIP_STRING_LEN];
            strncpy(colNum, element + 1, colNumLen);
            colNum[colNumLen] = '\0';

            if (isAllDigits(colNum) && isAllDigits(rowStr + 1)) {
              int col = atoi(colNum);
              int row = atoi(rowStr + 1);

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
    int capitalizedIdx = 0;

    for (int i = 0; input[i] != '\0' && capitalizedIdx < SKIP_STRING_LEN - 1; i++) {
      char c = input[i];

      // Capitalize c to C and r to R
      if (c == 'c') c = 'C';
      if (c == 'r') c = 'R';

      capitalized[capitalizedIdx++] = c;
    }
    capitalized[capitalizedIdx] = '\0';

    strncpy(output, capitalized, SKIP_STRING_LEN - 1);
    output[SKIP_STRING_LEN - 1] = '\0';

    // Step 2: Remove all unnecessary characters
    char normalized[SKIP_STRING_LEN];
    int normalizedIdx = 0;
    bool hasInvalidChar = false;
    char firstInvalidChar = '\0';

    for (int i = 0; capitalized[i] != '\0' && normalizedIdx < SKIP_STRING_LEN - 1; i++) {
      char c = capitalized[i];

      // Keep only valid characters based on type
      bool isValidChar = false;
      if (type == COLUMN) {
        // Valid characters: C, digits, comma
        isValidChar = (c == 'C' || (c >= '0' && c <= '9') || c == ',');
      } else if (type == ROW) {
        // Valid characters: R, digits, comma
        isValidChar = (c == 'R' || (c >= '0' && c <= '9') || c == ',');
      } else if (type == INDIVIDUAL) {
        // Valid characters: C, R, digits, comma
        isValidChar = (c == 'C' || c == 'R' || (c >= '0' && c <= '9') || c == ',');
      }

      if (isValidChar) {
        normalized[normalizedIdx++] = c;
      } else if (!hasInvalidChar) {
        // Record first invalid character for error message
        hasInvalidChar = true;
        firstInvalidChar = c;
      }
    }
    normalized[normalizedIdx] = '\0';

    if (hasInvalidChar && error_message != nullptr) {
      sprintf(error_message, "Error: Invalid character \"%c\" detected.", firstInvalidChar);
      return;
    }

    if (normalized[0] == '\0') {
      return;
    }

    // Step 3: Parse and validate format (CXX, RXX, or CXRX where X is a digit)
    char result[SKIP_STRING_LEN];
    result[0] = '\0';
    bool firstEntry = true;
    bool hasInvalidFormat = false;
    char firstInvalidToken[SKIP_STRING_LEN];
    firstInvalidToken[0] = '\0';

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

      bool isValid = false;

      if (type == COLUMN) {
        // Format: C followed by digits (CXX where X is a digit)
        if (token[0] == 'C' && strlen(token) > 1 && isAllDigits(token + 1)) {
          int num = atoi(token + 1);
          if (num > 0) {
            isValid = true;
            if (!firstEntry) strcat(result, ",");
            strcat(result, token);
            firstEntry = false;
          }
        }
      } else if (type == ROW) {
        // Format: R followed by digits (RXX where X is a digit)
        if (token[0] == 'R' && strlen(token) > 1 && isAllDigits(token + 1)) {
          int num = atoi(token + 1);
          if (num > 0) {
            isValid = true;
            if (!firstEntry) strcat(result, ",");
            strcat(result, token);
            firstEntry = false;
          }
        }
      } else if (type == INDIVIDUAL) {
        // Format: C followed by digits, then R followed by digits (CXRX where X is a digit)
        if (token[0] == 'C') {
          char* rowStr = strchr(token + 1, 'R');
          if (rowStr != nullptr) {
            // Extract column number between C and R
            int colNumLen = rowStr - (token + 1);
            if (colNumLen > 0) {
              char colNum[SKIP_STRING_LEN];
              strncpy(colNum, token + 1, colNumLen);
              colNum[colNumLen] = '\0';

              // Verify both parts are all digits
              if (isAllDigits(colNum) && isAllDigits(rowStr + 1)) {
                int col = atoi(colNum);
                int row = atoi(rowStr + 1);

                if (col > 0 && row > 0) {
                  isValid = true;
                  if (!firstEntry) strcat(result, ",");
                  strcat(result, token);
                  firstEntry = false;
                }
              }
            }
          }
        }
      }

      // Record first invalid token for error message
      if (!isValid && !hasInvalidFormat) {
        hasInvalidFormat = true;
        strncpy(firstInvalidToken, token, SKIP_STRING_LEN - 1);
        firstInvalidToken[SKIP_STRING_LEN - 1] = '\0';
      }

      token = strtok(nullptr, ",");
    }

    // Set error message for invalid format if no invalid character was detected
    if (hasInvalidFormat && error_message != nullptr && error_message[0] == '\0') {
      sprintf(error_message, "Error: Invalid format \"%s\" detected.", firstInvalidToken);
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
    char formatError[SKIP_STRING_LEN];
    format(input, type, formatted, formatError);

    // As long as provided a formatted string, return it later
    if (formatted[0] != '\0') {
      strncpy(result.cleaned, formatted, SKIP_STRING_LEN - 1);
      result.cleaned[SKIP_STRING_LEN - 1] = '\0';
    }

    // If there was a format error, return it with the formatted (capitalized) string
    if (formatError[0] != '\0') {
      strncpy(result.error_message, formatError, SKIP_STRING_LEN - 1);
      result.error_message[SKIP_STRING_LEN - 1] = '\0';
      return result;
    }

    // Step 2: Parse formatted string and check bounds
    char temp[SKIP_STRING_LEN];
    strncpy(temp, formatted, SKIP_STRING_LEN - 1);
    temp[SKIP_STRING_LEN - 1] = '\0';

    char finalResult[SKIP_STRING_LEN];
    finalResult[0] = '\0';
    bool firstEntry = true;
    bool hasOutOfBounds = false;
    char firstOutOfBoundsToken[SKIP_STRING_LEN];
    firstOutOfBoundsToken[0] = '\0';

    char* token = strtok(temp, ",");
    while (token != nullptr) {
      // Convert element to Position
      TrayHandler::Position pos = convertElement(token, type);

      // Check if position is valid and within bounds
      if (isValidSkipPosition(pos, dimensions, staggered)) {
        if (!firstEntry) strcat(finalResult, ",");
        strcat(finalResult, token);
        firstEntry = false;
      } else if (!hasOutOfBounds) {
        // Record first out-of-bounds token for error message
        hasOutOfBounds = true;
        strncpy(firstOutOfBoundsToken, token, SKIP_STRING_LEN - 1);
        firstOutOfBoundsToken[SKIP_STRING_LEN - 1] = '\0';
      }

      token = strtok(nullptr, ",");
    }

    // If there was an out-of-bounds error, return it with the cleaned string
    if (hasOutOfBounds) {
      sprintf(result.error_message, "Error: Position \"%s\" is out of bounds.", firstOutOfBoundsToken);
      return result;
    }

    // Copy final result to output
    strncpy(result.cleaned, finalResult, SKIP_STRING_LEN - 1);
    result.cleaned[SKIP_STRING_LEN - 1] = '\0';

    // Check if input was modified
    result.was_cleaned = (strcmp(input, result.cleaned) != 0);

    return result;
  }
};
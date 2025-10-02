#ifndef SKIP_UTILS_H
#define SKIP_UTILS_H

#include <Arduino.h>
#include "../../Profile.h"

// Define MAX_POSITIONS if not already defined
#ifndef MAX_POSITIONS
#define MAX_POSITIONS 100
#endif

// Forward declaration - Position must be defined before including this header
namespace TrayHandler {
  struct Position;
}

class SkipUtils {
public:
  enum SkipType {
    ROW,
    COLUMN,
    INDIVIDUAL
  };

  struct CleanResult {
    char cleaned[ROW_COL_MAX_LEN];
    bool wasCleaned;

    CleanResult() : wasCleaned(false) {
      cleaned[0] = '\0';
    }
  };

private:
  // Helper function to check if a string contains only digits.
  // @param str The string to check.
  // @return True if string contains only digits, false otherwise.
  static bool isAllDigits(const char *str) {
    if (str == nullptr || str[0] == '\0') return false;
    for (int i = 0; str[i] != '\0'; i++) {
      if (str[i] < '0' || str[i] > '9') return false;
    }
    return true;
  }

public:

  // Converts profile skip strings into an array of Position objects.
  // @param profile The profile containing skip information.
  // @param outPositions Output array to store parsed positions (must be at least MAX_POSITIONS in size).
  // @return The number of positions parsed.
  static int convert(Profile &profile, TrayHandler::Position outPositions[MAX_POSITIONS]) {
    // Initialize all positions to -1 to mark unused entries
    for (int i = 0; i < MAX_POSITIONS; i++) {
      outPositions[i] = TrayHandler::Position(-1, -1);
    }

    int posIndex = 0;

    // Parse skipCol string (format: "C1,C9,...")
    if (profile.skipCol[0] != '\0') {
      char tempCol[ROW_COL_MAX_LEN];
      strncpy(tempCol, profile.skipCol, ROW_COL_MAX_LEN - 1);
      tempCol[ROW_COL_MAX_LEN - 1] = '\0';

      char *token = strtok(tempCol, ",");
      while (token != nullptr && posIndex < MAX_POSITIONS) {
        // Extract column number (skip the 'C' prefix)
        if (token[0] == 'C') {
          int col = atoi(token + 1);
          if (col > 0) {
            // Add a position with x=col, y=0 to indicate entire column should be skipped
            outPositions[posIndex++] = TrayHandler::Position(col, 0);
          }
        }
        token = strtok(nullptr, ",");
      }
    }

    // Parse skipRow string (format: "R1,R9,...")
    if (profile.skipRow[0] != '\0') {
      char tempRow[ROW_COL_MAX_LEN];
      strncpy(tempRow, profile.skipRow, ROW_COL_MAX_LEN - 1);
      tempRow[ROW_COL_MAX_LEN - 1] = '\0';

      char *token = strtok(tempRow, ",");
      while (token != nullptr && posIndex < MAX_POSITIONS) {
        // Extract row number (skip the 'R' prefix)
        if (token[0] == 'R') {
          int row = atoi(token + 1);
          if (row > 0) {
            // Add a position with x=0, y=row to indicate entire row should be skipped
            outPositions[posIndex++] = TrayHandler::Position(0, row);
          }
        }
        token = strtok(nullptr, ",");
      }
    }

    // Parse skipSinglePos string (format: "C2R4,C3R4,...")
    if (profile.skipSinglePos[0] != '\0') {
      char tempPos[ROW_COL_MAX_LEN];
      strncpy(tempPos, profile.skipSinglePos, ROW_COL_MAX_LEN - 1);
      tempPos[ROW_COL_MAX_LEN - 1] = '\0';

      char *token = strtok(tempPos, ",");
      while (token != nullptr && posIndex < MAX_POSITIONS) {
        // Extract column and row numbers (format: CxRy)
        int col = 0, row = 0;
        char *colStr = strstr(token, "C");
        char *rowStr = strstr(token, "R");

        if (colStr && rowStr) {
          // Extract column number
          col = atoi(colStr + 1);

          // Extract row number
          row = atoi(rowStr + 1);

          if (col > 0 && row > 0) {
            // Add a position with specific x,y coordinates
            outPositions[posIndex++] = TrayHandler::Position(col, row);
          }
        }
        token = strtok(nullptr, ",");
      }
    }

    return posIndex;
  }

  // Cleans and validates a skip string by removing invalid characters and malformed entries.
  // @param input The input string to clean.
  // @param type The type of skip string (ROW, COLUMN, or INDIVIDUAL).
  // @return CleanResult containing the cleaned string and whether it was modified.
  static CleanResult clean(const char *input, SkipType type) {
    CleanResult result;
    
    if (input == nullptr) {
      return result;
    }
    
    // Step 1: Capitalize 'c' to 'C' and 'r' to 'R', remove all unnecessary characters
    char normalized[ROW_COL_MAX_LEN];
    int normalizedIdx = 0;
    
    for (int i = 0; input[i] != '\0' && normalizedIdx < ROW_COL_MAX_LEN - 1; i++) {
      char c = input[i];
      
      // Capitalize c to C and r to R
      if (c == 'c') c = 'C';
      if (c == 'r') c = 'R';
      
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
      }
    }
    normalized[normalizedIdx] = '\0';
    
    if (normalized[0] == '\0') {
      result.wasCleaned = (strcmp(input, "") != 0);
      return result;
    }

    // Step 2: Parse and validate format (CXX, RXX, or CXRX where X is a digit)
    char cleanedStr[ROW_COL_MAX_LEN];
    cleanedStr[0] = '\0';
    bool firstEntry = true;

    char temp[ROW_COL_MAX_LEN];
    strncpy(temp, normalized, ROW_COL_MAX_LEN - 1);
    temp[ROW_COL_MAX_LEN - 1] = '\0';

    char *token = strtok(temp, ",");
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
            if (!firstEntry) strcat(cleanedStr, ",");
            strcat(cleanedStr, token);
            firstEntry = false;
          }
        }
      } else if (type == ROW) {
        // Format: R followed by digits (RXX where X is a digit)
        if (token[0] == 'R' && strlen(token) > 1 && isAllDigits(token + 1)) {
          int num = atoi(token + 1);
          if (num > 0) {
            isValid = true;
            if (!firstEntry) strcat(cleanedStr, ",");
            strcat(cleanedStr, token);
            firstEntry = false;
          }
        }
      } else if (type == INDIVIDUAL) {
        // Format: C followed by digits, then R followed by digits (CXRX where X is a digit)
        if (token[0] == 'C') {
          char *rowStr = strchr(token + 1, 'R');
          if (rowStr != nullptr) {
            // Extract column number between C and R
            int colNumLen = rowStr - (token + 1);
            if (colNumLen > 0) {
              char colNum[ROW_COL_MAX_LEN];
              strncpy(colNum, token + 1, colNumLen);
              colNum[colNumLen] = '\0';
              
              // Verify both parts are all digits
              if (isAllDigits(colNum) && isAllDigits(rowStr + 1)) {
                int col = atoi(colNum);
                int row = atoi(rowStr + 1);
                
                if (col > 0 && row > 0) {
                  isValid = true;
                  if (!firstEntry) strcat(cleanedStr, ",");
                  strcat(cleanedStr, token);
                  firstEntry = false;
                }
              }
            }
          }
        }
      }

      token = strtok(nullptr, ",");
    }

    strncpy(result.cleaned, cleanedStr, ROW_COL_MAX_LEN - 1);
    result.cleaned[ROW_COL_MAX_LEN - 1] = '\0';
    
    // Check if the cleaned string differs from the input
    result.wasCleaned = (strcmp(input, result.cleaned) != 0);
    
    return result;
  }
};

#endif // SKIP_UTILS_H
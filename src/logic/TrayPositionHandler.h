#pragma once

#include <Arduino.h>
#include "../../Config.h"
#include "../Constants.h"

namespace TrayHandler {

struct Dimensions {
  int columns;
  int rows;

  Dimensions() : columns(0), rows(0) {}
  Dimensions(int cols, int rows) : columns(cols), rows(rows) {}
};

struct Position {
  int x;
  int y;

  Position(int x = -1, int y = -1) : x(x), y(y) {}

  bool operator==(const Position& other) const { return x == other.x && y == other.y; }
};

}  // namespace TrayHandler

// Include SkipUtils after Position is defined
#include "SkipUtils.h"

namespace TrayHandler {

struct PositionResult {
  bool has_next;  // Whether all valid positions have been exhausted

  // These fields are only valid when has_next is true
  Position position;  // The next position
  int direction;      // The direction to move in

  // Static factory method for "done" state
  static PositionResult Done() { return PositionResult(Position(-1, -1), false, 0); }

  // Static factory method for valid position state
  static PositionResult Valid(const Position& pos, int dir) { return PositionResult(pos, true, dir); }

 private:
  // Private constructor used by the static factory methods
  PositionResult(Position pos, bool has_next, int dir) : position(pos), has_next(has_next), direction(dir) {}
};

class TrayPositionHandler {
 private:
  Dimensions dimensions;
  Position current_position = Position(1, 1);
  int direction = 1;
  bool staggered = false;
  bool flipped = false;

  Position skip_positions[MAX_SKIP_POSITIONS] = { Position(-1, -1) };

  // Cached values calculated during reset
  int total_valid_tubes = 0;
  int tubes_dispensed = 0;

  // Calculate the next valid position within the current row.
  // @return PositionResult.
  PositionResult getNextInRow(const Position& start_pos, const int dir) {
    Position next_pos = start_pos;
    do {
      // Advance to next position based on current direction
      next_pos.x += dir;

      // Handle reaching the end of the row
      if (next_pos.x > dimensions.columns || next_pos.x < 1) {
        // Reached end of row without finding a valid position
        return PositionResult::Done();
      }

    } while (isInvalidPosition(next_pos));

    // Return the next valid position
    return PositionResult::Valid(next_pos, dir);
  }

  PositionResult getNextInNewRow(const Position& start_pos) {
    // Find first and last non-skipped positions in the row directly
    Position first(0, start_pos.y);
    Position last(0, start_pos.y);
    int valid_count = 0;

    // Single pass through the row to find first and last valid positions
    for (int x = 1; x <= dimensions.columns; x++) {
      Position current_pos(x, start_pos.y);
      if (!isInvalidPosition(current_pos)) {
        // If this is the first valid position found, set both first and last
        if (valid_count == 0) {
          first = current_pos;
          last = current_pos;
        } else {
          // Otherwise, update only the last position
          last = current_pos;
        }
        valid_count++;
      }
    }

    // Return based on the number of valid positions found
    if (valid_count == 0) {
      // No valid positions in this row
      return PositionResult::Done();
    } else if (valid_count == 1) {
      // Only one valid position, return it
      return PositionResult::Valid(first, direction);
    } else {
      // Multiple valid positions, find closest one
      // Calculate distances from current position to first and last
      int dist_to_first = start_pos.x - first.x;
      if (dist_to_first < 0) dist_to_first = -dist_to_first;

      int dist_to_last = start_pos.x - last.x;
      if (dist_to_last < 0) dist_to_last = -dist_to_last;

      // Return the closest one and set direction accordingly
      if (dist_to_first <= dist_to_last) {
        return PositionResult::Valid(first, 1);  // Moving right
      } else {
        return PositionResult::Valid(last, -1);  // Moving left
      }
    }
  }

  // Calculate the next valid position in the grid.
  // @return PositionResult.
  PositionResult getNext() {
    // Check in current row
    PositionResult result = getNextInRow(current_position, direction);
    if (result.has_next) return result;

    // Find the next row with a valid position
    for (int y = current_position.y + 1; y <= dimensions.rows; y++) {
      // Find a valid position in the new row
      PositionResult result = getNextInNewRow(Position(current_position.x, y));
      if (result.has_next) return result;
    }
    return PositionResult::Done();
  }

  // Check if a position should be skipped.
  // @param pos Position to check.
  // @return True if position should be skipped, false otherwise.
  bool isInvalidPosition(const Position& pos) {
    for (int i = 0; i < MAX_SKIP_POSITIONS; i++) {
      if (pos.x == -1 || pos.y == -1) {
        break;
      }
      if (pos.x == skip_positions[i].x && pos.y == skip_positions[i].y) {
        return true;
      }
      // Skip entire column
      if (pos.x == skip_positions[i].x && skip_positions[i].y == 0) {
        return true;
      }
      // Skip entire row
      if (pos.y == skip_positions[i].y && skip_positions[i].x == 0) {
        return true;
      }
      // If stagger is enabled, skip every last position in even rows (or even columns if flipped)
      if (staggered) {
        if (flipped) {
          if (pos.x % 2 == 0 && pos.y == dimensions.rows) {
            return true;
          }
        } else {
          if (pos.y % 2 == 0 && pos.x == dimensions.columns) {
            return true;
          }
        }
      }
    }
    return false;
  }

  bool shouldFlip(Position skips[MAX_SKIP_POSITIONS]) {
    // If there are more vertical skips than horizontal skips, flip the direction
    int vertical_skips = 0;
    int horizontal_skips = 0;
    for (int i = 0; i < MAX_SKIP_POSITIONS; i++) {
      if (skips[i].x == -1 || skips[i].y == -1) {
        break;
      }
      if (skips[i].x == 0) horizontal_skips++;
      if (skips[i].y == 0) vertical_skips++;
    }
    return vertical_skips > horizontal_skips;
  }

  Position flipPosition(const Position& pos) const { return Position(pos.y, pos.x); }

  // Copy skip positions and flip them if needed.
  // @param positions Array of positions to transform.
  // @param out_positions Output array to store flipped positions.
  // @param flip Whether to flip the positions.
  void copyPositions(const Position positions[MAX_SKIP_POSITIONS], Position out_positions[MAX_SKIP_POSITIONS], bool flip) {
    for (int i = 0; i < MAX_SKIP_POSITIONS; i++) {
      if (positions[i].x == -1 || positions[i].y == -1) {
        out_positions[i] = Position(-1, -1);
        break;
      }
      out_positions[i] = flip ? flipPosition(positions[i]) : positions[i];
    }
  }

  Dimensions flipDimensions(Dimensions d) { return Dimensions(d.rows, d.columns); }

 public:
  TrayPositionHandler() = default;
  ~TrayPositionHandler() = default;

  void loadProfile(Profile& profile) {
    // Use SkipUtils to parse skip positions from profile
    Position positions[MAX_SKIP_POSITIONS];
    SkipUtils::convert(profile, positions);

    // Create dimensions from profile
    Dimensions dimensions(profile.tube_no_x, profile.tube_no_y);

    // Load the dimensions and skip positions
    load(dimensions, positions, profile.staggered);
  }

  // Load tray configuration.
  // @param dimensions Dimensions of the tray grid.
  // @param positions Array of positions to skip/ignore.
  void load(const Dimensions& dimensions, const Position new_positions[MAX_SKIP_POSITIONS], bool staggered) {
    this->staggered = staggered;
    flipped = shouldFlip(new_positions);

    this->dimensions = flipped ? flipDimensions(dimensions) : dimensions;
    copyPositions(new_positions, skip_positions, flipped);
  }

  // Get the skip positions array.
  // @param out_positions Output array to copy skip positions to.
  void getSkipPositions(Position out_positions[MAX_SKIP_POSITIONS]) const {
    copyPositions(skip_positions, out_positions, flipped);
  }

  // Get the next valid position and update the current position.
  // @return PositionResult containing the new position
  PositionResult goToNextValidPosition() {
    // Store the previous position before updating
    Position previous_position = current_position;
    Position previous_position_transformed = flipped ? flipPosition(previous_position) : previous_position;

    PositionResult result = getNext();

    // If we have a valid next position, update the current position and transform if needed
    if (result.has_next) {
      // Update the current position to the new position
      current_position = result.position;
      direction = result.direction;

      // Increment tubes dispensed counter
      tubes_dispensed++;

      // Transform the result position if the grid is flipped
      Position new_position;
      if (flipped) {
        new_position = flipPosition(result.position);
      } else {
        new_position = result.position;
      }

      result.position = new_position;
    }

    return result;
  }

  // Reset the current position to the bottom-left corner.
  // @return The reset position in the original coordinate system.
  Position reset() {
    current_position = Position(1, 1);
    direction = 1;

    if (isInvalidPosition(current_position)) {
      current_position = getNext().position;
    }

    tubes_dispensed = 0;

    // Calculate total valid tubes once
    int staggered_rows = staggered ? dimensions.rows / 2 : 0;
    int total_tubes = dimensions.rows * dimensions.columns - staggered_rows;
    int skip_count = 0;

    // Count skip positions - track which rows/columns are already skipped to avoid double-counting
    bool skipped_rows[TUBES_Y_MAX + 1] = { false };
    bool skipped_cols[TUBES_X_MAX + 1] = { false };

    // First pass: mark entire rows and columns as skipped
    for (int i = 0; i < MAX_SKIP_POSITIONS; i++) {
      if (skip_positions[i].x == -1 || skip_positions[i].y == -1) break;

      // Mark entire column as skipped
      if (skip_positions[i].y == 0 && skip_positions[i].x > 0) {
        if (!skipped_cols[skip_positions[i].x]) {
          skipped_cols[skip_positions[i].x] = true;
          int rows_in_col = staggered && skip_positions[i].x == dimensions.columns ? dimensions.rows / 2 : dimensions.rows;
          skip_count += rows_in_col;
        }
      }
      // Mark entire row as skipped
      else if (skip_positions[i].x == 0 && skip_positions[i].y > 0) {
        if (!skipped_rows[skip_positions[i].y]) {
          skipped_rows[skip_positions[i].y] = true;
          int cols_in_row = staggered && skip_positions[i].y % 2 == 0 ? dimensions.columns - 1 : dimensions.columns;
          skip_count += cols_in_row;
        }
      }
    }

    // Second pass: count individual skip positions only if not already in a skipped row/column
    for (int i = 0; i < MAX_SKIP_POSITIONS; i++) {
      if (skip_positions[i].x == -1 || skip_positions[i].y == -1) break;

      // Count individual skip positions only if not in a skipped row or column
      if (skip_positions[i].x > 0 && skip_positions[i].y > 0) {
        if (!skipped_rows[skip_positions[i].y] && !skipped_cols[skip_positions[i].x]) {
          skip_count++;
        }
      }
    }

    total_valid_tubes = total_tubes - skip_count;

    return current_position;
  }

  // Get the current row (1-indexed).
  // @return Current row position.
  int getCurrentRow() const {
    Position pos = flipped ? flipPosition(current_position) : current_position;
    return pos.y;
  }

  // Get the current column (1-indexed).
  // @return Current column position.
  int getCurrentColumn() const {
    Position pos = flipped ? flipPosition(current_position) : current_position;
    return pos.x;
  }

  // Calculate the number of tubes left to process.
  // @return Number of remaining tubes.
  int getTubesLeft() const { return total_valid_tubes - tubes_dispensed; }

  // Get the number of tubes dispensed so far.
  // @return Number of tubes dispensed.
  int getTubesDispensed() const { return tubes_dispensed; }
};

}  // namespace TrayHandler

#ifndef TRAY_POSITION_HANDLER_H
#define TRAY_POSITION_HANDLER_H

#include <Arduino.h>


#define MAX_POSITIONS 100

namespace TrayHandler {

struct Dimensions {
  int columns;
  int rows;

  Dimensions()
    : columns(0), rows(0) {}
  Dimensions(int cols, int rows)
    : columns(cols), rows(rows) {}
};

struct Position {
  int x;
  int y;

  Position(int x = 0, int y = 0)
    : x(x), y(y) {}

  bool operator==(const Position &other) const {
    return x == other.x && y == other.y;
  }
};

}  // namespace TrayHandler

// Include SkipUtils after Position is defined
#include "SkipUtils.h"

namespace TrayHandler {

struct PositionResult {
  bool hasNext;  // Whether all valid positions have been exhausted

  // These fields are only valid when hasNext is true
  Position position;  // The next position
  int direction;      // The direction to move in

  // Static factory method for "done" state
  static PositionResult Done() {
    return PositionResult(Position(0, 0), false, 0);
  }

  // Static factory method for valid position state
  static PositionResult Valid(const Position &pos, int dir) {
    return PositionResult(pos, true, dir);
  }

private:
  // Private constructor used by the static factory methods
  PositionResult(Position pos, bool hasNext, int dir)
    : position(pos), hasNext(hasNext), direction(dir) {}
};

class TrayPositionHandler {
private:
  Dimensions dimensions;
  Position currentPosition = Position(1, 1);
  int direction = 1;
  bool flipped = false;

  Position skipPositions[MAX_POSITIONS];

  // Calculate the next valid position within the current row.
  // @return PositionResult.
  PositionResult getNextInRow(const Position &startPos, const int dir) {
    Position nextPos = startPos;
    do {
      // Advance to next position based on current direction
      nextPos.x += dir;

      // Handle reaching the end of the row
      if (nextPos.x > dimensions.columns || nextPos.x < 1) {
        // Reached end of row without finding a valid position
        return PositionResult::Done();
      }

    } while (isSkipPosition(nextPos));

    // Return the next valid position
    return PositionResult::Valid(nextPos, dir);
  }

  PositionResult getNextInNewRow(const Position &startPos) {
    // Find first and last non-skipped positions in the row directly
    Position first(0, startPos.y);
    Position last(0, startPos.y);
    int validCount = 0;

    // Single pass through the row to find first and last valid positions
    for (int x = 1; x <= dimensions.columns; x++) {
      Position currentPos(x, startPos.y);
      if (!isSkipPosition(currentPos)) {
        // If this is the first valid position found, set both first and last
        if (validCount == 0) {
          first = currentPos;
          last = currentPos;
        } else {
          // Otherwise, update only the last position
          last = currentPos;
        }
        validCount++;
      }
    }

    // Return based on the number of valid positions found
    if (validCount == 0) {
      // No valid positions in this row
      return PositionResult::Done();
    } else if (validCount == 1) {
      // Only one valid position, return it
      return PositionResult::Valid(first, direction);
    } else {
      // Multiple valid positions, find closest one
      // Calculate distances from current position to first and last
      int distToFirst = startPos.x - first.x;
      if (distToFirst < 0) distToFirst = -distToFirst;

      int distToLast = startPos.x - last.x;
      if (distToLast < 0) distToLast = -distToLast;

      // Return the closest one and set direction accordingly
      if (distToFirst <= distToLast) {
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
    PositionResult result = getNextInRow(currentPosition, direction);
    if (result.hasNext)
      return result;

    // Find the next row with a valid position
    for (int y = currentPosition.y + 1; y <= dimensions.rows; y++) {
      // Find a valid position in the new row
      PositionResult result = getNextInNewRow(Position(currentPosition.x, y));
      if (result.hasNext)
        return result;
    }
    return PositionResult::Done();
  }

  // Check if a position should be skipped.
  // @param pos Position to check.
  // @return True if position should be skipped, false otherwise.
  bool isSkipPosition(const Position &pos) {
    for (int i = 0; i < MAX_POSITIONS; i++) {
      if (pos.x == -1 || pos.y == -1) { break; }
      if (pos.x == skipPositions[i].x && pos.y == skipPositions[i].y) { return true; }
      // Skip entire column
      if (pos.x == skipPositions[i].x && skipPositions[i].y == 0) { return true; }
      // Skip entire row
      if (pos.y == skipPositions[i].y && skipPositions[i].x == 0) { return true; }
    }
    return false;
  }

  bool shouldFlip() {
    // If there are more vertical skips than horizontal skips, flip the direction
    int verticalSkips = 0;
    int horizontalSkips = 0;
    for (int i = 0; i < MAX_POSITIONS; i++) {
      if (skipPositions[i].x == -1 || skipPositions[i].y == -1) { break; }
      if (skipPositions[i].x == 0)
        horizontalSkips++;
      if (skipPositions[i].y == 0)
        verticalSkips++;
    }
    return verticalSkips > horizontalSkips;
  }

  Position flipPosition(const Position &pos) const {
    return Position(pos.y, pos.x);
  }

  void transformSkipPositions() {
    for (int i = 0; i < MAX_POSITIONS; i++) {
      if (skipPositions[i].x == -1 || skipPositions[i].y == -1) { break; }
      skipPositions[i] = flipPosition(skipPositions[i]);
    }
  }

  Dimensions flipDimensions(Dimensions d) {
    return Dimensions(d.rows, d.columns);
  }

public:
  TrayPositionHandler() = default;
  ~TrayPositionHandler() = default;

  void load_profile(Profile &profile) {
    // Use SkipUtils to parse skip positions from profile
    Position positions[MAX_POSITIONS];
    SkipUtils::convert(profile, positions);

    // Create dimensions from profile
    Dimensions dimensions(profile.Tube_No_x, profile.Tube_No_y);

    // Load the dimensions and skip positions
    load(dimensions, positions);
  }

  // Load tray configuration.
  // @param dimensions Dimensions of the tray grid.
  // @param positions Array of positions to skip/ignore.
  void load(const Dimensions &dimensions,
            const Position positions[MAX_POSITIONS]) {

    flipped = shouldFlip();

    this->dimensions = flipped ? flipDimensions(dimensions) : dimensions;

    for (int i = 0; i < MAX_POSITIONS; i++) {
      if (positions[i].x == -1 || positions[i].y == -1) { break; }
      skipPositions[i] = positions[i];
    }
    if (flipped) transformSkipPositions();
  }

  // Get the skip positions array.
  // @param outPositions Output array to copy skip positions to.
  void getSkipPositions(Position outPositions[MAX_POSITIONS]) const {
    for (int i = 0; i < MAX_POSITIONS; i++) {
      outPositions[i] = skipPositions[i];
    }
  }

  // Get the next valid position and update the current position.
  // @return PositionResult containing the new position
  PositionResult goToNextValidPosition() {
    // Store the previous position before updating
    Position previousPosition = currentPosition;
    Position previousPositionTransformed = flipped ? flipPosition(previousPosition) : previousPosition;

    PositionResult result = getNext();

    // If we have a valid next position, update the current position and transform if needed
    if (result.hasNext) {
      // Update the current position to the new position
      currentPosition = result.position;
      direction = result.direction;

      // Transform the result position if the grid is flipped
      Position newPosition;
      if (flipped) {
        newPosition = flipPosition(result.position);
      } else {
        newPosition = result.position;
      }

      result.position = newPosition;
    }

    return result;
  }

  // Reset the current position to the bottom-left corner.
  // @return The reset position in the original coordinate system.
  Position reset() {
    currentPosition = Position(1, 1);
    direction = 1;
    return currentPosition;
  }

  // Get the current row (1-indexed).
  // @return Current row position.
  int getCurrentRow() const {
    Position pos = flipped ? flipPosition(currentPosition) : currentPosition;
    return pos.y;
  }

  // Get the current column (1-indexed).
  // @return Current column position.
  int getCurrentColumn() const {
    Position pos = flipped ? flipPosition(currentPosition) : currentPosition;
    return pos.x;
  }

  // Calculate the number of tubes left to process.
  // @return Number of remaining tubes.
  int getTubesLeft() const {
    int totalTubes = dimensions.rows * dimensions.columns;
    int skipCount = 0;
    
    // Count skip positions
    for (int i = 0; i < MAX_POSITIONS; i++) {
      if (skipPositions[i].x == -1 || skipPositions[i].y == -1) break;
      
      // Count individual skip positions
      if (skipPositions[i].x > 0 && skipPositions[i].y > 0) {
        skipCount++;
      }
      // Count entire column skips
      else if (skipPositions[i].y == 0 && skipPositions[i].x > 0) {
        skipCount += dimensions.rows;
      }
      // Count entire row skips
      else if (skipPositions[i].x == 0 && skipPositions[i].y > 0) {
        skipCount += dimensions.columns;
      }
    }
    
    // Calculate processed tubes (current row * columns + current column - 1)
    int processedTubes = (currentPosition.y - 1) * dimensions.columns + currentPosition.x;
    
    // Total valid tubes
    int validTubes = totalTubes - skipCount;
    
    // Remaining tubes
    return validTubes - processedTubes;
  }
};

}  // namespace TrayHandler

#endif  // TRAY_POSITION_HANDLER_H

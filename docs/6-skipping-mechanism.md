# Skipping Mechanism

This document describes the skipping mechanism that allows users to exclude specific positions from the dispense cycle. The system supports three types of skip definitions:

- **Skip Columns** - Exclude entire columns from dispensing
- **Skip Rows** - Exclude entire rows from dispensing  
- **Skip Individual Positions** - Exclude specific cells by their coordinates

The skipping mechanism is configured through the Advanced Settings screen and is validated to ensure the skip parameters are within acceptable limits.

## Skip Types

### Skip Columns

Users can specify one or more column indices to skip during the dispense cycle. Columns are specified as comma-separated values.

**Example:** `C1,C5,C10` will skip columns 1, 5, and 10.

**Maximum:** Up to 42 columns can be skipped (defined by `MAX_SKIP_POSITIONS_COLUMNS` in `Constants.h`).

### Skip Rows

Users can specify one or more row indices to skip during the dispense cycle. Rows are specified as comma-separated values.

**Example:** `R2,R7,R15` will skip rows 2, 7, and 15.

**Maximum:** Up to 33 rows can be skipped (defined by `MAX_SKIP_POSITIONS_ROWS` in `Constants.h`).

### Skip Individual Positions

Users can specify individual cell positions to skip using coordinate notation. Each position is specified as `(column,row)` and multiple positions are separated by semicolons.

**Example:** `C3R4,C8R12,C15R20` will skip cells at column 3 row 4, column 8 row 12, and column 15 row 20.

**Maximum:** Up to 30 individual positions can be skipped (defined by `MAX_SKIP_POSITIONS_INDIVIDUAL` in `Constants.h`).

\newpage
## Total Skip Limit

The system enforces a **total maximum of 100 skip positions** across all three skip types combined (defined by `MAX_SKIP_POSITIONS_TOTAL` in `Constants.h`). This limit (and the individual limits) primarily arises from limited memory resources for the string buffer used to store the skip parameters, and the limited storage space available in the microcontroller's EEPROM.

### Calculation

The total skip count is calculated as:

```
Total Skips = (Number of Skipped Columns) + (Number of Skipped Rows) +
(Number of Individual Positions)
```

**Note:** When a column or row is skipped, it counts as a single skip position regardless of how many cells it contains. Individual cell skips each count as one position.

\newpage

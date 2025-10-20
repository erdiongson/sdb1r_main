# Error Codes

This document lists all error codes and their meanings.

## Error Reference Table

| Error Code | Explanation |
|:-----------|:------------|
| E01 - IR Sensor Error | Passed-through from the PIC. |
| E02 - IR Marker Error | Passed-through from the PIC. |
| E11 - Dispenser Error | The PIC did not receive an ACK from the dispenser head within the timeout period. |
| E12 - Dispenser Timeout Error | The PIC did not receive a "Cycle Complete" signal from the dispenser head within the timeout period. |
| E13 - Limit Switch Triggered | A limit switch was triggered unexpectedly, indicating a possible movement boundary collision. |
| E21 - Parameter Error | One or more parameters in the tray configuration are outside the valid range. |
| E22 - Dimension Error | The tray dimensions calculated from the parameters are outside the valid range. |
| E23 - Skip Values Error | Skip positions are invalid for current tray configuration. |
| E24 - Too Many Skip Positions | The total number of skip positions exceeds the maximum allowed. |
| E99 - Unknown Error | An unknown error occurred. This means the error code is not defined in the code. |

\newpage

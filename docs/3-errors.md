# Error Codes

This document lists all error codes and their meanings.

## Error Reference Table

| Error Code | Explanation |
|:-----------|:------------|
| E01 - IR Sensor Error | The system failed to detect the dispenser head using the IR sensor. This error originates from the PIC microcontroller and typically requires a device restart. |
| E02 - IR Marker Error | The system failed to detect the dispenser head using the IR marker. This error originates from the PIC microcontroller and typically requires a device restart. |
| E11 - Dispenser Error | The system could not detect the dispenser head. Communication with the dispenser failed, and a device restart is required. |
| E12 - Dispenser Timeout Error | The system did not receive a response from the dispenser head within the expected timeout period. A device restart is required. |
| E13 - Limit Switch Triggered | A limit switch was triggered unexpectedly, indicating a possible movement boundary collision. The operator should check the head position before resuming operation. |
| E15 - Unlatching Error | The system could not unlatch the dispenser head. The movement system should be checked before restarting the device. |
| E16 - Homing Timeout Error | The system could not home the dispenser head within the expected timeout period. The movement system should be checked before restarting the device. |
| E17 - Zeroing Timeout Error | The system could not reach the zero position within the expected timeout period. The movement system should be checked before resuming operation. |
| E21 - Parameter Error | One or more tray configuration parameters are outside the valid range. The system highlights the invalid values that need correction. |
| E22 - Dimension Error | The calculated tray dimensions from the provided parameters exceed the physical tray boundaries. The configuration values must be verified and corrected. |
| E23 - Skip Values Error | The configured skip positions are invalid for the current tray configuration. The skip settings must be updated in the Advanced settings menu. |
| E24 - Excessive Skips Error | The total number of configured skip positions exceeds the system's maximum allowed limit. The number of skip positions must be reduced. |
| E25 - No Valid Positions Error | All positions on the grid have been marked as skipped, leaving no available positions for dispensing. At least one position must be available for operation. |
| E99 - Unknown Error | An unrecognized error code was encountered. This indicates an error condition not defined in the system's error handling code. |

\newpage

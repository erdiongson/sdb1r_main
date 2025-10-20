# Dispense Cycle Flow

This document describes the dispense cycle managed by the RunController, which orchestrates the entire dispensing operation through a series of stages from initialization to completion. This process includes:

- Setting the vibration level and duration on the dispenser head
- Priming the dispenser head
- Calculating the most efficient path to fill all required positions
- Controlling the Z axis to "dip" the dispenser head into the tray

The dispense cycle is broken down into **stages**, to ensure that the system can pause and resume the cycle at any point, and to allow for proper error handling.


## Stages Overview

The dispense cycle consists of 11 stages that execute sequentially:

1. **Idle** - Initial idle state, all motors stopped
2. **Set Vibration Level** - Configure vibration level on dispenser
3. **Set Vibration Duration** - Configure vibration duration on dispenser
4. **Zero** - Move all axes to zero position (home)
5. **Start Prime** - Send prime command to dispenser
6. **Wait Prime** - Wait for prime cycle to complete
7. **Move** - Move XY axes to target position
8. **Lower Head** - Lower Z axis to dip depth
9. **Start Dispense** - Send dispense command to dispenser
10. **Wait Dispense** - Wait for dispense cycle to complete
11. **Raise Head** - Raise Z axis back to zero position

## Error Handling

During any stage, the following errors will trigger a dialog and cause certain actions:

| Error Code | Description |
|:-----------|:------------|
| E01 - IR Sensor Failure | Pause cycle indefinitely and request user to restart the device |
| E02 - Marker Not Detected | Pause cycle indefinitely and request user to restart the device |
| E11 - ACK Error | Pause cycle indefinitely and request user to restart the device |
| E12 - Cycle Timeout | Pause cycle indefinitely and request user to restart the device |
| E13 - Limit Switch Triggered | Pause cycle and allow user to resume or stop the dispense cycle |

When paused, the user can:

- **Resume**: Continue from the current stage
- **Stop**: Abort the cycle and return to home position

\newpage

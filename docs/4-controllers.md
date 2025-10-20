# Controller Architecture

## Controller Interface

At any point in time, one controller is active on the system, handling the logic of the entire system.

The controller might simply be waiting for user input, or running the stepper motors until a certain position is reached, or waiting for the dispenser to finish a dispense cycle.

Controllers share a common interface, which allows them to interact with the rest of the system in a predictable way.

Controllers can signal that another controller should take over by using the `startNewController` callback.

## Controller Methods

| Method | Caller | Description |
|:-------|:-------|:------------|
| `onStart` | ControllerManager | Triggers initial logic when controller becomes active |
| `onInteraction` | InteractionHandler | Handles button presses and PLC commands |
| `onStep` | Main Loop | Runs stepper motors, processes dispenser commands/responses, handles internal state machine |

## Controller Dependencies

Controllers receive the following dependencies during initialization:

- `dispenserHead`: Axis movement and dispenser commands
- `profileManager`: Get/update profiles
- `phost`: Update UI display

## Controller Lifecycle

1. **Initialization**: Controller is created with injected dependencies (`dispenserHead`, `profileManager`, `phost`)
2. **Activation**: `ControllerManager` calls `onStart()` to trigger initial logic
3. **Event Loop**:
   - `InteractionHandler` calls `onInteraction()` for button presses and PLC commands
   - Main loop continuously calls `onStep()` for motor control, dispenser communication, and state management
4. **Transition**: Controller calls `startNextController(<con_id>)` callback to transfer control to another controller

\newpage

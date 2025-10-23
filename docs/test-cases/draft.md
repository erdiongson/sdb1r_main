# Startup Process
Dispenser connectivity is checked
Axis are homed

If dispenser is not connected, the system will display an error message
If a limit switch is triggered, the system will pause the action and display an error message

# Profile System
Profile parameters can be set and saved (up to 10)
Profiles can be loaded and used
Profile selection is persistent on reboot
Profile parameter minimum / maximum values are checked and enforced

Default password is correctly set and can be used
Master password can be used
Password can be set and used
Password can be enabled and disabled on a per-profile basis
If the password is wrong, the system will display an error message
If during password change, the second password does not match the first, the system will display an error message

Staggered mode can be set and used

# Skipping Mechanism
Skipped columns can be set and saved (to maximum of 42)
Skipped rows can be set and saved (to maximum of 33)
Skipped individuals can be set and saved (to maximum of 30)

If more than 100 skipped positions are set, the system will display an error message

# Dispensing Process
Dispense cycle can be started and stopped
Dispense cycle can be paused and resumed at any point (except dispensing)

Dispenser primes at the start of the cycle
Dispenser is homed at the end of the cycle
Dispenser performs a "z-dip" if configured

Dispenser does not stop at skipped positions
Dispenser shows the correct number of dispensed tubes and remaining tubes

If no valid positions are found on start, the system will stop the cycle immediately and display an error message
If the dispenser (PIC) returns an error, the system will stop the cycle and display an error message
If a limit switch is triggered, the system will pause the cycle and display an error message

# Homing Process
Axis are homed
If a limit switch is triggered, the system will pause the action and display an error message

# PLC Communication
The system acts correctly to a START command when at the "Ready" state
The system acts correctly to a STOP command when at the "Run" state
The system acts correctly to a PAUSE command when at the "Run" state

The system acts correctly to a RAISE_Z command when at the "Ready" state
The system acts correctly to a LOWER_Z command when at the "Ready" state

If the system receives a START command when at the "Run" state, it will return BUSY
If the system receives a message with an invalid checksum, it will return NAK
If the system receives a message with an unknown command, it will return NAK
The system responds with a mirrored acknowledge for all other valid cases

The system sends a COMPLETED message when the system has finished processing
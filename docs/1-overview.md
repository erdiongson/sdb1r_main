# XentiQ 1R Technical Documentation

## System Overview

This repository contains the source code for the Arduino controller for the XentiQ 1R machine, which communicates with the PIC microcontroller via UART. The Arduino controller is responsible for the following:

- Front-end display and user interface (using the EVE display)
- Stepper motor control and limit switch monitoring
- Profile management
- PIC microcontroller communication
- PLC communication

## System Architecture

The codebase is organized into the following modules:

| Module | Description |
|:-------|:------------|
| `controllers/` | State machine controllers for different UI screens and workflows |
| `views/` | Display rendering components for UI elements |
| `hardware/` | Low-level hardware interfaces (motors, limit switches) |
| `serial/` | UART communication protocols for PIC interface |
| `logic/` | Business logic for profile handling and dispensing operations |
| `gpu/` | Graphics processing utilities for the EVE display |

## Configuration

System configuration is centralized in **`Config.h`**, which contains all hardware and operational parameters:

### Key Configuration Parameters
- **Motor Settings**: Speed, acceleration, and steps-per-unit for X/Y/Z axes
- **Limit Switch Pins**: Hardware pin assignments for safety limits
- **Dispenser Timeouts**: ACK timeout, cycle timeout
- **Profile Defaults**: Origin position, pitch spacing, Z-dip depth, cycle count
- **Security**: Password settings for system access
- **Debug Flags**: Enable/disable logging, screen-only mode, memory monitoring

\newpage

# PLC Serial

## Overview

This guide explains how to communicate with the system using PLC serial commands. The system uses a simple, fixed-length binary protocol over a serial connection for reliable command transmission.

## Hardware Setup

### Connection Requirements

- **Baud Rate:** 19200
- **Data Format:** 8 data bits, 1 stop bit, no parity (8N1)
- **Flow Control:** None

---

## Message Protocol

### Message Structure

Every message consists of exactly **5 bytes** in the following format:

```
┌─────────┬─────────┬──────┬──────────┬─────────┐
│  START  │ COMMAND │ DATA │ CHECKSUM │   END   │
│  0xEF   │ 1 byte  │1 byte│  1 byte  │  0xFE   │
└─────────┴─────────┴──────┴──────────┴─────────┘
  Byte 0    Byte 1   Byte 2   Byte 3    Byte 4
```

| Byte | Field | Value | Description |
|------|-------|-------|-------------|
| 0 | START_BYTE | `0xEF` | Fixed start delimiter |
| 1 | COMMAND | Variable | Command code (see command table) |
| 2 | DATA | Variable | Command parameter (use `0x00` if not needed) |
| 3 | CHECKSUM | Calculated | Sum of COMMAND + DATA (masked to 8 bits) |
| 4 | END_BYTE | `0xFE` | Fixed end delimiter |

### Checksum Calculation

The checksum ensures message integrity:

```
CHECKSUM = (COMMAND + DATA) & 0xFF
```

**Example:**
- COMMAND = `0x30`
- DATA = `0x00`
- CHECKSUM = `(0x30 + 0x00) & 0xFF` = `0x30`

---

## Available Commands

### Command Reference Table

| Command | Code | Data Byte | Hex Message | Description |
|---------|------|-----------|-------------|-------------|
| **START** | `0x30` | `0x00` | `EF 30 00 30 FE` | Start system operation |
| **STOP** | `0x31` | `0x00` | `EF 31 00 31 FE` | Stop system operation |
| **PAUSE** | `0x32` | `0x00` | `EF 32 00 32 FE` | Pause current operation |
| **RAISE_Z** | `0x42` | Variable | `EF 42 XX CS FE` | Raise Z-axis (XX = value in mm) |
| **LOWER_Z** | `0x41` | Variable | `EF 41 XX CS FE` | Lower Z-axis (XX = value in mm) |


---

## System Responses

### Acknowledgment (ACK)

When a valid command is received and accepted, the system **mirrors back** the exact same 5-byte message:

**Example:**
```
PLC sends:     EF 30 00 30 FE  (START command)
System replies: EF 30 00 30 FE  (ACK - same message)
```

### Negative Acknowledgment (NAK)

The system sends a NAK response for:
- **Invalid checksum:** Checksum does not match calculated value
- **Unknown command:** Command byte is not in the supported command list

**NAK Response Format:**
```
15 15 15 15 15  (five NAK bytes: 0x15)
```

**Action:** Verify message format and checksum, then retransmit.

### Busy Response (BUSY)

If a START command is sent while the system is busy, the system responds with:

```
16 16 16 16 16  (five BUSY bytes: 0x16)
```

**Action:** Wait for system to complete current operation before retrying.

### No Response

The system will **not respond** if:
- Message format is invalid (wrong start/end bytes)
- Message length is not exactly 5 bytes
- First byte is not the START_BYTE

**Action:** Verify message format and retransmit.

---

## System Status Messages

### COMPLETED Message

When the system completes an operation, it sends:

```
EF 17 00 17 FE
```

**Breakdown:**
- START: `0xEF`
- COMMAND: `0x17` (completion indicator)
- DATA: `0x00`
- CHECKSUM: `0x17`
- END: `0xFE`

**Usage:** Monitor for this message to know when the system has finished processing.

---
# Screenshot Capture Utility

This utility allows you to capture screenshots from the SDB1R display system via serial communication.

## Requirements

- Python 3.6 or higher
- Required Python packages:
  ```bash
  pip install pyserial pillow
  ```

## Usage

### Basic Usage

```bash
python capture_screen.py
```

This will:
1. Connect to `/dev/ttyUSB0` at 9600 baud (default)
2. Send the "SCREENSHOT" command to the Arduino
3. Receive the RGB565 image data
4. Convert and save as PNG in the `screenshots/` folder
5. Filename format: `screenshot_YYYYMMDD_HHMMSS.png`

### Custom Serial Port

**Linux/Mac:**
```bash
python capture_screen.py --port /dev/ttyUSB0
```

**Windows:**
```bash
python capture_screen.py --port COM3
```

### Custom Baud Rate

```bash
python capture_screen.py --baud 115200
```

### Custom Output Directory

```bash
python capture_screen.py --output my_screenshots
```

### All Options

```bash
python capture_screen.py --port COM3 --baud 9600 --output screenshots --timeout 10
```

## Command Line Options

- `--port`, `-p`: Serial port (default: `/dev/ttyUSB0`)
- `--baud`, `-b`: Baud rate (default: `9600`)
- `--output`, `-o`: Output directory (default: `screenshots`)
- `--timeout`, `-t`: Serial timeout in seconds (default: `10`)

## How It Works

1. **Arduino Side:**
   - The main loop checks for "SCREENSHOT" command on Serial
   - When received, it uses `CMD_SNAPSHOT` to capture the display to RAM_G
   - Sends back: header, dimensions, size, RGB565 pixel data, completion marker

2. **Python Side:**
   - Sends "SCREENSHOT" ASCII command
   - Receives acknowledgment and image metadata
   - Receives RGB565 pixel data in chunks
   - Converts RGB565 to RGB888 format
   - Saves as PNG with timestamp

## Protocol Details

**Command:** `SCREENSHOT` (10 ASCII bytes)

**Response Format:**
1. Header: `SNAP` (4 bytes)
2. Width: 2 bytes (little-endian uint16)
3. Height: 2 bytes (little-endian uint16)
4. Total bytes: 4 bytes (little-endian uint32)
5. Pixel data: RGB565 format (2 bytes per pixel)
6. Completion: `DONE` (4 bytes)

## Troubleshooting

### "Error connecting to port"
- Check if the correct serial port is specified
- Ensure the Arduino is connected and powered
- Check port permissions (Linux/Mac): `sudo chmod 666 /dev/ttyUSB0`

### "Timeout while receiving data"
- Increase timeout: `--timeout 30`
- Check serial connection stability
- Verify baud rate matches Arduino configuration

### "PIL (Pillow) is required"
```bash
pip install Pillow
```

### "Permission denied" (Linux/Mac)
```bash
sudo usermod -a -G dialout $USER
# Then log out and log back in
```

## Display Configurations

The system supports multiple display sizes:
- **3.5" IPS:** 320x240 pixels (153,600 bytes)
- **7" IPS:** 1024x600 pixels (1,228,800 bytes)

The Python script automatically handles different resolutions.

## Example Output

```
$ python capture_screen.py --port /dev/ttyUSB0
Connected to /dev/ttyUSB0 at 9600 baud
Sent command: SCREENSHOT
Received acknowledgment
Image dimensions: 1024x600
Expecting 1228800 bytes (1228800 calculated)
Receiving image data... 100.0%
Received 1228800 bytes successfully
Transfer complete
Screenshot saved: screenshots/screenshot_20251129_151345.png
Disconnected
```

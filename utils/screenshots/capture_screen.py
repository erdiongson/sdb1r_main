#!/usr/bin/env python3
"""
Screenshot Capture Utility for SDB1R Display System

Connects to the Arduino via serial port, sends "SCREENSHOT" command,
and receives RGB565 image data. Saves the screenshot as a PNG file
with timestamp in the screenshots folder.

Usage:
    python capture_screen.py [--port PORT] [--baud BAUD]

Example:
    python capture_screen.py --port /dev/ttyUSB0 --baud 9600
    python capture_screen.py --port COM3 --baud 9600
"""

import serial
import struct
import time
from datetime import datetime
from pathlib import Path
import argparse
import sys

try:
    from PIL import Image
except ImportError:
    print("Error: PIL (Pillow) is required. Install with: pip install Pillow")
    sys.exit(1)


class ScreenshotCapture:
    """Handles screenshot capture from Arduino display system."""
    
    def __init__(self, port, baudrate=115200, timeout=10):
        """
        Initialize screenshot capture.
        
        Args:
            port: Serial port name (e.g., '/dev/ttyUSB0' or 'COM3')
            baudrate: Serial communication speed (default: 115200)
            timeout: Serial read timeout in seconds (default: 10)
        """
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.ser = None
        
    def connect(self):
        """Connect to the serial port."""
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=self.timeout,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE
            )
            print(f"Connected to {self.port} at {self.baudrate} baud")
            print("Waiting for Arduino to boot...")
            time.sleep(4)  # Increased wait time for Arduino reset and initialization
            
            # Clear any boot messages/logs from the buffer
            self.ser.reset_input_buffer()
            time.sleep(0.5)  # Small additional delay
            print("Ready to capture")
            return True
        except serial.SerialException as e:
            print(f"Error connecting to {self.port}: {e}")
            return False
    
    def disconnect(self):
        """Disconnect from the serial port."""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("Disconnected")
    
    def send_command(self):
        """Send SCREENSHOT command to Arduino."""
        if not self.ser or not self.ser.is_open:
            print("Error: Not connected to serial port")
            return False
        
        # Clear any pending data
        self.ser.reset_input_buffer()
        
        # Send command
        command = b"SCREENSHOT"
        self.ser.write(command)
        print(f"Sent command: {command.decode()}")
        return True
    
    def receive_screenshot(self):
        """
        Receive screenshot data from Arduino.
        
        Returns:
            tuple: (width, height, pixel_data) or None on error
        """
        # Wait for acknowledgment header "SNAP"
        header = self.ser.read(4)
        if header != b"SNAP":
            print(f"Error: Invalid header received: {header}")
            return None
        
        print("Received acknowledgment")
        
        # Read dimensions (2 bytes each, little-endian)
        width_bytes = self.ser.read(2)
        height_bytes = self.ser.read(2)
        
        if len(width_bytes) != 2 or len(height_bytes) != 2:
            print("Error: Failed to read dimensions")
            return None
        
        width = struct.unpack('<H', width_bytes)[0]
        height = struct.unpack('<H', height_bytes)[0]
        
        print(f"Image dimensions: {width}x{height}")
        
        # Read total bytes (4 bytes, little-endian)
        size_bytes = self.ser.read(4)
        if len(size_bytes) != 4:
            print("Error: Failed to read size")
            return None
        
        total_bytes = struct.unpack('<I', size_bytes)[0]
        expected_bytes = width * height * 2  # RGB565 = 2 bytes per pixel
        
        print(f"Expecting {total_bytes} bytes ({expected_bytes} calculated)")
        
        # Read pixel data
        pixel_data = bytearray()
        bytes_received = 0
        
        print("Receiving image data...", end='', flush=True)
        
        while bytes_received < total_bytes:
            chunk = self.ser.read(min(1024, total_bytes - bytes_received))
            if not chunk:
                print(f"\nError: Timeout while receiving data (got {bytes_received}/{total_bytes} bytes)")
                return None
            
            pixel_data.extend(chunk)
            bytes_received += len(chunk)
            
            # Progress indicator
            if bytes_received % 10240 == 0:
                progress = (bytes_received / total_bytes) * 100
                print(f"\rReceiving image data... {progress:.1f}%", end='', flush=True)
        
        print(f"\rReceived {bytes_received} bytes successfully")
        
        # Wait for completion marker "DONE"
        done_marker = self.ser.read(4)
        if done_marker == b"DONE":
            print("Transfer complete")
        else:
            print(f"Warning: Expected 'DONE' marker, got: {done_marker}")
        
        return (width, height, bytes(pixel_data))
    
    def rgb565_to_rgb888(self, rgb565_data, width, height):
        """
        Convert RGB565 data to RGB888 format.
        EVE chip uses RGB565 format for snapshots (better quality than ARGB4444).
        
        Args:
            rgb565_data: Raw RGB565 bytes (little-endian)
            width: Image width
            height: Image height
            
        Returns:
            bytes: RGB888 data
        """
        rgb888_data = bytearray()
        
        for i in range(0, len(rgb565_data), 2):
            # Read 16-bit value as little-endian
            # RGB565 format: RRRRRGGGGGGBBBBB (5-6-5 bits)
            rgb565 = struct.unpack('<H', rgb565_data[i:i+2])[0]
            
            # Extract RGB components
            r5 = (rgb565 >> 11) & 0x1F  # 5 bits red
            g6 = (rgb565 >> 5) & 0x3F   # 6 bits green
            b5 = rgb565 & 0x1F          # 5 bits blue
            
            # Scale to 8-bit with proper bit replication for smooth gradients
            r8 = (r5 << 3) | (r5 >> 2)  # Scale 5-bit to 8-bit
            g8 = (g6 << 2) | (g6 >> 4)  # Scale 6-bit to 8-bit
            b8 = (b5 << 3) | (b5 >> 2)  # Scale 5-bit to 8-bit
            
            rgb888_data.extend([r8, g8, b8])
        
        return bytes(rgb888_data)
    
    def save_image(self, width, height, rgb565_data, output_path):
        """
        Save screenshot as PNG image.
        
        Args:
            width: Image width
            height: Image height
            rgb565_data: Raw RGB565 pixel data
            output_path: Path to save the image
        """
        # Convert RGB565 to RGB888
        rgb888_data = self.rgb565_to_rgb888(rgb565_data, width, height)
        
        # Create PIL Image
        image = Image.frombytes('RGB', (width, height), rgb888_data)
        
        # Save as PNG
        image.save(output_path, 'PNG')
        print(f"Screenshot saved: {output_path}")
    
    def capture(self, output_dir="screenshots"):
        """
        Capture a screenshot and save it.
        
        Args:
            output_dir: Directory to save screenshots (default: "screenshots")
            
        Returns:
            bool: True if successful, False otherwise
        """
        # Create output directory if it doesn't exist
        output_path = Path(output_dir)
        output_path.mkdir(exist_ok=True)
        
        # Wait for user to press Enter
        print("\nPress Enter to capture screenshot...")
        input()
        
        # Send screenshot command
        if not self.send_command():
            return False
        
        # Receive screenshot data
        result = self.receive_screenshot()
        if not result:
            return False
        
        width, height, pixel_data = result
        
        # Generate filename with timestamp
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"screenshot_{timestamp}.png"
        filepath = output_path / filename
        
        # Save image
        self.save_image(width, height, pixel_data, filepath)
        
        return True


def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Capture screenshots from SDB1R display system via serial port"
    )
    parser.add_argument(
        '--port', '-p',
        default='/dev/ttyUSB0',
        help='Serial port (default: /dev/ttyUSB0)'
    )
    parser.add_argument(
        '--baud', '-b',
        type=int,
        default=115200,
        help='Baud rate (default: 115200)'
    )
    parser.add_argument(
        '--output', '-o',
        default='screenshots',
        help='Output directory (default: screenshots)'
    )
    parser.add_argument(
        '--timeout', '-t',
        type=int,
        default=10,
        help='Serial timeout in seconds (default: 10)'
    )
    
    args = parser.parse_args()
    
    # Create capture instance
    capture = ScreenshotCapture(
        port=args.port,
        baudrate=args.baud,
        timeout=args.timeout
    )
    
    # Connect to serial port
    if not capture.connect():
        return 1
    
    try:
        # Continuous capture loop
        while True:
            success = capture.capture(output_dir=args.output)
            if not success:
                print("Screenshot failed. Press Enter to retry or Ctrl+C to exit...")
                try:
                    input()
                except KeyboardInterrupt:
                    break
    
    except KeyboardInterrupt:
        print("\nExiting...")
        return 0
    
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    finally:
        capture.disconnect()


if __name__ == '__main__':
    sys.exit(main())

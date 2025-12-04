#include "ScreenshotHandler.h"

bool ScreenshotHandler::checkForCommand() {
    // Check if "SCREENSHOT" command is available on Serial
    if (Serial.available() >= 10) {
        char buffer[11];
        Serial.readBytes(buffer, 10);
        buffer[10] = '\0';
        
        if (strcmp(buffer, "SCREENSHOT") == 0) {
            return true;
        }
    }
    return false;
}

void ScreenshotHandler::captureAndSend(Gpu_Hal_Context_t* phost) {
    // Get display dimensions from the configuration
    uint16_t width = DispWidth;
    uint16_t height = DispHeight;
    
    // Calculate total bytes (RGB565 = 2 bytes per pixel)
    uint32_t total_bytes = (uint32_t)width * height * 2;
    
    // Send acknowledgment header
    Serial.write("SNAP");
    
    // Send dimensions and size
    sendUint16(width);
    sendUint16(height);
    sendUint32(total_bytes);
    
    // Capture screenshot to RAM_G using RGB565 format for better quality
    // CMD_SNAPSHOT2 allows specifying format (RGB565 has more color depth than ARGB4444)
    Gpu_CoCmd_Snapshot2(phost, RGB565, SNAPSHOT_RAM_ADDR, 0, 0, width, height);
    Gpu_Hal_WaitCmdfifo_empty(phost);
    
    // Read and transmit data in chunks
    uint8_t buffer[CHUNK_SIZE];
    uint32_t bytes_sent = 0;
    
    while (bytes_sent < total_bytes) {
        uint16_t chunk_size = CHUNK_SIZE;
        if (bytes_sent + chunk_size > total_bytes) {
            chunk_size = total_bytes - bytes_sent;
        }
        
        // Read chunk from RAM_G
        Gpu_Hal_RdMem(phost, SNAPSHOT_RAM_ADDR + bytes_sent, buffer, chunk_size);
        
        // Send chunk over Serial
        Serial.write(buffer, chunk_size);
        
        bytes_sent += chunk_size;
    }
    
    // Send completion marker
    Serial.write("DONE");
    Serial.flush();
}

void ScreenshotHandler::sendUint32(uint32_t value) {
    Serial.write((uint8_t)(value & 0xFF));
    Serial.write((uint8_t)((value >> 8) & 0xFF));
    Serial.write((uint8_t)((value >> 16) & 0xFF));
    Serial.write((uint8_t)((value >> 24) & 0xFF));
}

void ScreenshotHandler::sendUint16(uint16_t value) {
    Serial.write((uint8_t)(value & 0xFF));
    Serial.write((uint8_t)((value >> 8) & 0xFF));
}

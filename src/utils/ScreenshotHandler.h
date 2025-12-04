#pragma once

#include "../gpu/Platform.h"

/**
 * Handles screenshot capture and transmission over Serial.
 * Uses CMD_SNAPSHOT to capture the current display to RAM_G,
 * then transmits the RGB565 data over Serial.
 */
class ScreenshotHandler {
public:
    /**
     * Captures a screenshot and sends it over Serial.
     * @param phost Pointer to GPU HAL context
     */
    static void captureAndSend(Gpu_Hal_Context_t* phost);

    /**
     * Checks if a screenshot command has been received on Serial.
     * @return true if "SCREENSHOT" command was received
     */
    static bool checkForCommand();

private:
    static const uint32_t SNAPSHOT_RAM_ADDR = 0; // Address in RAM_G to store snapshot
    static const uint16_t CHUNK_SIZE = 512;      // Bytes to read/send at a time
    
    /**
     * Sends a 32-bit value over Serial in little-endian format.
     */
    static void sendUint32(uint32_t value);
    
    /**
     * Sends a 16-bit value over Serial in little-endian format.
     */
    static void sendUint16(uint16_t value);
};

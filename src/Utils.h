#pragma once
#include "../Config.h"
#include "Constants.h"

// Shared buffer for logging to avoid String concatenations and heap allocations
// Size is set to accommodate the longest log message in the codebase
static char g_log_buffer[KEYBOARD_MAX_LEN];

#if DEBUG_NO_LOG == 0

// Static class for simple logging via Serial.
class Logger {
 public:
  // Logs a message to Serial output.
  // @param x The message string to log.
  static void log(String x) { Serial.println(x); }

  // Logs two strings concatenated.
  // @param x The first string.
  // @param y The second string.
  static void log(String x, String y) {
    Serial.print(x);
    Serial.println(y);
  }

  // Logs a string followed by a float value.
  // @param x The string prefix.
  // @param y The float value to log.
  static void log(String x, float y) {
    char temp_buffer[20];  // Max: "-999.99999"
    dtostrf(y, 3, 5, temp_buffer);
    Serial.print(x);
    Serial.println(temp_buffer);
  }

  // Logs a string followed by an unsigned 8-bit integer.
  // @param x The string prefix.
  // @param y The uint8_t value to log.
  static void log(String x, uint8_t y) {
    char temp_buffer[4];  // Max: "255"
    sprintf(temp_buffer, "%d", y);
    Serial.print(x);
    Serial.println(temp_buffer);
  }

  // Logs a string followed by a byte array in hexadecimal format.
  // @param prefix The string prefix.
  // @param data Pointer to the byte array.
  // @param length Number of bytes to log.
  static void log(String prefix, const uint8_t* data, size_t length) {
    Serial.print(prefix);
    for (size_t i = 0; i < length; i++) {
      Serial.print(data[i], HEX);
      if (i < length - 1) {
        Serial.print(" ");
      }
    }
    Serial.println();
  }
};

#else

// Static class for simple logging via Serial (no-op version).
class Logger {
 public:
  // Logs a message to Serial output.
  // @param x The message string to log.
  static void log(String x) {}

  // Logs two strings concatenated.
  // @param x The first string.
  // @param y The second string.
  static void log(String x, String y) {}

  // Logs a string followed by a float value.
  // @param x The string prefix.
  // @param y The float value to log.
  static void log(String x, float y) {}

  // Logs a string followed by an unsigned 8-bit integer.
  // @param x The string prefix.
  // @param y The uint8_t value to log.
  static void log(String x, uint8_t y) {}

  // Logs a string followed by a byte array in hexadecimal format.
  // @param prefix The string prefix.
  // @param data Pointer to the byte array.
  // @param length Number of bytes to log.
  static void log(String prefix, const uint8_t* data, size_t length) {}
};

#endif

#if DEBUG_NO_LOG == 0

// Static class for monitoring and printing memory statistics.
class MemoryMonitor {
 private:
  inline static unsigned long last_print_time = 0;

 public:
  // Prints the free memory available on the Arduino.
  static void printFreeMemory() {
    extern int __heap_start, *__brkval;
    int v;
    int free_memory = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);

    char temp_buffer[80];  // Max: "Free memory: -32768 bytes | Stack: 0xFFFF | Heap: 0xFFFF | Uptime: 4294967295s"
    sprintf(temp_buffer, "Free memory: %d bytes | Stack: 0x%X | Heap: 0x%X | Uptime: %lus", free_memory, (int)&v,
            __brkval == 0 ? (int)&__heap_start : (int)__brkval, millis() / 1000);
    Logger::log(String(temp_buffer));
  }

  // Prints memory stats periodically based on the specified interval.
  // @param interval_ms The interval in milliseconds between prints.
  static void printPeriodically(unsigned long interval_ms) {
    unsigned long current_time = millis();
    if (current_time - last_print_time >= interval_ms) {
      last_print_time = current_time;
      printFreeMemory();
    }
  }
};

#else

// Static class for monitoring and printing memory statistics (no-op version).
class MemoryMonitor {
 private:
  inline static unsigned long last_print_time = 0;

 public:
  // Prints the free memory available on the Arduino.
  static void printFreeMemory() {}

  // Prints memory stats periodically based on the specified interval.
  // @param interval_ms The interval in milliseconds between prints.
  static void printPeriodically(unsigned long interval_ms) {}
};

#endif
#pragma once
#include "../Config.h"


// Static class for simple logging via Serial.
class Logger {
public:
  // Logs a message to Serial output.
  // @param x The message string to log.
  static void log(String x) {
    Serial.println(x);
  }

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
    char buf[20];
    dtostrf(y, 3, 5, buf);
    Serial.print(x);
    Serial.println(buf);
  }

  // Logs a string followed by an unsigned 8-bit integer.
  // @param x The string prefix.
  // @param y The uint8_t value to log.
  static void log(String x, uint8_t y) {
    char buf[20];
    sprintf(buf, "%d", y);
    Serial.print(x);
    Serial.println(buf);
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


// Static class for monitoring and printing memory statistics.
class MemoryMonitor {
private:
  inline static unsigned long last_print_time = 0;

public:
  // Prints the free memory available on the Arduino.
  static void printFreeMemory() {
    extern int __heap_start, *__brkval;
    int v;
    int free_memory = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    
    Serial.print("Free memory: ");
    Serial.print(free_memory);
    Serial.print(" bytes | Stack: 0x");
    Serial.print((int)&v, HEX);
    Serial.print(" | Heap: 0x");
    Serial.print(__brkval == 0 ? (int)&__heap_start : (int)__brkval, HEX);
    Serial.print(" | Uptime: ");
    Serial.print(millis() / 1000);
    Serial.println("s");
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
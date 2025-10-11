#pragma once
#include "../Config.h"

void inline Dprint(char x) { Serial.println(x, HEX); }
void inline Dprint(String x) { Serial.println(x); }
void inline Dprint(String x, String y) {
  Serial.print(x);
  Serial.print(y);
}
void inline Dprint(String x, float y) {
  char buf[20];

  dtostrf(y, 3, 5, buf);
  Serial.print(x);
  Serial.println(buf);
}
void inline Dprint(String x, uint8_t y) {
  char buf[20];

  sprintf(buf, "%d", y);
  Serial.print(x);
  Serial.println(buf);
}

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

// #else

//   void inline Dprint(char x) {}
//   void inline Dprint(String x) {}
//   void inline Dprint(String x, String y) {}
//   void inline Dprint(String x, uint8_t y) {}
//   void inline Dprint(String x, float y) {}

// #endif
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

// #else

//   void inline Dprint(char x) {}
//   void inline Dprint(String x) {}
//   void inline Dprint(String x, String y) {}
//   void inline Dprint(String x, uint8_t y) {}
//   void inline Dprint(String x, float y) {}

// #endif
#include "funshield.h"

constexpr int leds[] = {led1_pin, led2_pin, led3_pin, led4_pin};
const int ledsCount = 4;


class Foo {
public:
  static void foo() {
    digitalWrite(leds[0], ON);
  }
};

void setup() {
  for (int i = 0; i < ledsCount; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], OFF);
  }
}

void loop() {
  unsigned long startTime = millis();
  constexpr unsigned long delayTime = 2000;
  if (min(startTime, delayTime) <= 1000) {
    digitalWrite(leds[0], ON);
  }
  Foo::foo();
}

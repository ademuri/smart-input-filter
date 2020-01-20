#include "filter.h"

void Filter::Run() {
  if (millis() >= run_at) {
    DoRun();
  }
}

#ifndef ARDUINO
void Filter::SetMillis(uint32_t value) { fake_millis = value; }

void Filter::SetPin(uint8_t pin, uint8_t value) { pins[pin] = value; }
#endif

void Filter::SetRunDelayInMillis(uint32_t delay) { run_at = millis() + delay; }

#ifndef ARDUINO
uint32_t Filter::millis() { return fake_millis; }

uint8_t Filter::digitalRead(uint8_t pin) { return pins[pin]; }
#endif

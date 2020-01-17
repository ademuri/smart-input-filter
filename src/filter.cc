#include "filter.h"

void Filter::Run() {
  if (millis() >= run_at) {
    DoRun();
  }
}

void Filter::SetRunDelayInMillis(uint32_t delay) {
  run_at = millis() + delay;
}

#ifndef ARDUINO
uint32_t Filter::millis() {
  return 0;
}

uint8_t Filter::digitalRead(uint8_t pin) {
  return 0;
}
#endif

#include "analog-filter.h"

AnalogFilter::AnalogFilter(uint32_t pin) : pin(pin) {}

uint32_t AnalogFilter::ReadFromSensor() {
  return analogRead(pin);
}

#ifndef ARDUINO
uint32_t AnalogFilter::analogRead(uint32_t pin) {
  (void)pin;
  return pin_value;
}
#endif


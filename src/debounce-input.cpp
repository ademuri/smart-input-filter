#include "debounce-input.h"

DebounceInput::DebounceInput(uint32_t pin) : pin(pin) {}

bool DebounceInput::Rose() { return rose_; }

bool DebounceInput::Fell() { return fell_; }

bool DebounceInput::ReadFromSensor() { return digitalRead(pin); }

bool DebounceInput::DoRun() {
  rose_ = false;
  fell_ = false;

  if (millis() - state_started_at_millis >= kDebounceTimeMillis) {
    if (stable_state_ != current_state_) {
      stable_state_ = current_state_;
      last_successful_change_at_millis = millis();

      if (stable_state_) {
        rose_ = true;
      } else {
        fell_ = true;
      }
    }
  }

  bool input_value = ReadFromSensor();
  if (input_value == current_state_) {
    return stable_state_;
  }

  if (millis() - last_successful_change_at_millis >= kDebounceTimeMillis) {
    if (stable_state_ != input_value) {
      if (input_value) {
        rose_ = true;
      } else {
        fell_ = true;
      }
    }

    stable_state_ = input_value;
    current_state_ = input_value;
    last_successful_change_at_millis = millis();
    state_started_at_millis = millis();
  }

  // Input != current_state_
  state_started_at_millis = millis();
  current_state_ = input_value;
  return stable_state_;
}

#ifndef ARDUINO
int DebounceInput::digitalRead(uint32_t pin) {
  (void)pin;
  return pin_value_;
}
#endif

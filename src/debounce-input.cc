#include "debounce-input.h"

DebounceInput::DebounceInput(uint16_t pin) : pin(pin) {}

bool DebounceInput::GetCurrentState() { return stable_state; }

bool DebounceInput::Rose() { return rose; }

bool DebounceInput::Fell() { return fell; }

void DebounceInput::DoRun() {
  rose = false;
  fell = false;

  if (millis() - state_started_at_millis >= kDebounceTimeMillis) {
    if (stable_state != current_state) {
      stable_state = current_state;
      last_successful_change_at_millis = millis();

      if (stable_state) {
        rose = true;
      } else {
        fell = true;
      }
    }
  }

  bool input_value = digitalRead(pin);
  if (input_value == current_state) {
    return;
  }

  if (millis() - last_successful_change_at_millis >= kDebounceTimeMillis) {
    if (stable_state != input_value) {
      if (input_value) {
        rose = true;
      } else {
        fell = true;
      }
    }

    stable_state = input_value;
    current_state = input_value;
    last_successful_change_at_millis = millis();
    state_started_at_millis = millis();
  }

  // Input != current_state
  state_started_at_millis = millis();
  current_state = input_value;
}

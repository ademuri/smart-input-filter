#include "debounce-input.h"

DebounceInput::DebounceInput(uint16_t pin) : pin(pin) {}

bool DebounceInput::GetCurrentState() { return stable_state; }

void DebounceInput::DoRun() {
  if (millis() - state_started_at_millis >= kDebounceTimeMillis) {
    if (stable_state != current_state) {
      stable_state = current_state;
      last_successful_change_at_millis = millis();
    }
  }

  bool input_value = digitalRead(pin);
  if (input_value == current_state) {
    return;
  }

  if (millis() - last_successful_change_at_millis >= kDebounceTimeMillis) {
    stable_state = input_value;
    current_state = input_value;
    last_successful_change_at_millis = millis();
    state_started_at_millis = millis();
  }

  // Input != current_state
  state_started_at_millis = millis();
  current_state = input_value;
}

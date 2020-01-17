#include "debounce-input.h"

DebounceInput::DebounceInput(uint16_t pin) : pin(pin) {}

bool DebounceInput::GetCurrentState() {
  return stable_state;
}

void DebounceInput::DoRun() {
  if (millis() - state_started_at_millis >= kDebounceTimeMillis) {
    stable_state = current_state;
  }

  bool input_value = digitalRead(pin);
  if (input_value == current_state) {
    return;
  }

  // Input != current_state
  state_started_at_millis = millis();
  current_state = input_value;
}

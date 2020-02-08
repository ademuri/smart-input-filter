#include "debounce-filter.h"

const uint8_t kButtonPin = 0;
const uint8_t kLedPin = 13;
DebounceFilter *button;

bool inverted = false;

bool readButtonPin() {
  if (inverted) {
    return !digitalRead(kButtonPin);
  } else {
    return digitalRead(kButtonPin);
  }
}

void setup() {
  pinMode(kButtonPin, INPUT_PULLUP);
  // Typical usage: with a normal digital input
  button = new DebounceFilter(filter_functions::ForDigitalRead<kButtonPin>());

  // Advanced usage: custom input function
  button = new DebounceFilter(readButtonPin);

  pinMode(kLedPin, OUTPUT);
}

void loop() {
  button->Run();
  if (button->Rose()) {
    Serial.println("You clicked a button!");
  }
  if (button->Fell()) {
    Serial.println("You released a button!");
  }

  digitalWrite(kLedPin, button->GetFilteredValue());
}

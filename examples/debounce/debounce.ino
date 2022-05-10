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
  // Typical usage: with a normal digital input, pin number available at compile
  // time
  button = new DebounceFilter(filter_functions::ForDigitalRead<kButtonPin>());

  // For an inverted input, e.g. when using a button with a pullup resistor
  button = new DebounceFilter(
      filter_functions::ForInvertedDigitalRead<kButtonPin>());

  // Like the above, but with the pin determined at runtime. Not available on
  // AVR.
#ifndef __AVR__
  button =
      new DebounceFilter(filter_functions::ForDigitalReadDynamic(random(10)));

  button = new DebounceFilter(
      filter_functions::ForInvertedDigitalReadDynamic(random(10)));
#endif  // __AVR__

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

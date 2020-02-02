#include "debounce-filter.h"

const uint8_t kButtonPin = 0;
const uint8_t kLedPin = 13;
DebounceFilter *button;

void setup() {
  pinMode(kButtonPin, INPUT_PULLUP);
  button = new DebounceFilter(kButtonPin);

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

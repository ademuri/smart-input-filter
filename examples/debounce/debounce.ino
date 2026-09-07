#include "debounce-filter.h"

const uint8_t kButtonPin = 0;
const uint8_t kLed1Pin = 13;
const uint8_t kLed2Pin = 14;

// Most users should use stack-allocated instances for better performance and
// reliability.
DebounceFilter button{filter_functions::ForDigitalRead<kButtonPin>()};

// This library also supports dynamic allocation using 'new'. While this is
// compiled here to ensure support, it is generally discouraged in embedded
// environments due to the risk of heap fragmentation.
DebounceFilter* buttonPtr;

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
  pinMode(kLed1Pin, OUTPUT);
  pinMode(kLed2Pin, OUTPUT);

  // Examples of dynamic allocation (for compilation testing):
  buttonPtr =
      new DebounceFilter(filter_functions::ForDigitalRead<kButtonPin>());

#ifndef __AVR__
  // Dynamic pin assignment at runtime (not available on AVR)
  delete buttonPtr;
  buttonPtr =
      new DebounceFilter(filter_functions::ForDigitalReadDynamic(random(10)));
#endif
}

void loop() {
  button.Run();

  // Use GetRawValue to get the current (possibly bouncing) state of the button
  // directly from the sensor.
  digitalWrite(kLed2Pin, button.GetRawValue());

  if (button.Rose()) {
    Serial.println("You clicked a button!");
  }
  if (button.Fell()) {
    Serial.println("You released a button!");
  }

  digitalWrite(kLed1Pin, button.GetFilteredValue());
}

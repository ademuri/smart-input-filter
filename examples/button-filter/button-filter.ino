#include "button-filter.h"

constexpr uint8_t kButtonPin = 0;
constexpr uint8_t kLedPin = 13;

// Most users should use stack-allocated instances for better performance and
// reliability.
ButtonFilter button{filter_functions::ForDigitalRead<kButtonPin>(),
                    /*held_time_millis=*/100};

// This library also supports dynamic allocation using 'new'. While this is
// compiled here to ensure support, it is generally discouraged in embedded
// environments due to the risk of heap fragmentation.
ButtonFilter *buttonPtr;

void setup() {
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);

  // Example of dynamic allocation (for compilation testing):
  buttonPtr =
      new ButtonFilter(filter_functions::ForDigitalRead<kButtonPin>(), 100);
}

void loop() {
  button.Run();
  if (button.Pressed()) {
    digitalWrite(kLedPin, HIGH);
    delay(100);
    digitalWrite(kLedPin, LOW);
  } else if (button.Held()) {
    digitalWrite(kLedPin, HIGH);
    delay(500);
    digitalWrite(kLedPin, LOW);
  }
}

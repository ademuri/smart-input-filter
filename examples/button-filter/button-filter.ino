#include "button-filter.h"

constexpr uint8_t kButtonPin = 0;
constexpr uint8_t kLedPin = 13;
ButtonFilter button{filter_functions::ForDigitalRead<kButtonPin>(),
                    /*held_time_millis=*/100};

void setup() {
  pinMode(kButtonPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
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

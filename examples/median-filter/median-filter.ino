#include "median-filter.h"

const uint8_t kInputPin = 0;
const uint8_t kLedPin = 13;
MedianFilter<uint32_t, uint32_t, 5> *filter;

void setup() {
  pinMode(kInputPin, INPUT_PULLUP);
  filter = new MedianFilter<uint32_t, uint32_t, 5>(
      filter_functions::ForAnalogRead<kInputPin>());

// Similar to the above, but with the input pin determined at runtime
#ifndef __AVR__
  filter = new MedianFilter<uint32_t, uint32_t, 5>(
      filter_functions::ForAnalogReadDynamic(random(10)));
#endif  // __AVR__

  filter->SetLogToSerial(true);

  pinMode(kLedPin, OUTPUT);
}

void loop() {
  filter->Run();
  digitalWrite(kLedPin, filter->GetFilteredValue() > 511);
  Serial.flush();
  delay(10);
}

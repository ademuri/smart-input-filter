#include "median-filter.h"

const uint8_t kInputPin = 0;
const uint8_t kLedPin = 13;

// Most users should use stack-allocated instances for better performance and
// reliability.
MedianFilter<uint32_t, uint32_t, 5> filter{
    filter_functions::ForAnalogRead<kInputPin>()};

// This library also supports dynamic allocation using 'new'. While this is
// compiled here to ensure support, it is generally discouraged in embedded
// environments due to the risk of heap fragmentation.
MedianFilter<uint32_t, uint32_t, 5>* filterPtr;

void setup() {
  pinMode(kInputPin, INPUT_PULLUP);

  filter.SetLogToSerial(true);

  pinMode(kLedPin, OUTPUT);

  // Example of dynamic allocation (for compilation testing):
  filterPtr = new MedianFilter<uint32_t, uint32_t, 5>(
      filter_functions::ForAnalogRead<kInputPin>());

#ifndef __AVR__
  // Example of dynamic allocation with runtime pin selection:
  delete filterPtr;
  filterPtr = new MedianFilter<uint32_t, uint32_t, 5>(
      filter_functions::ForAnalogReadDynamic(random(10)));
#endif
}

void loop() {
  filter.Run();
  digitalWrite(kLedPin, filter.GetFilteredValue() > 511);
  Serial.flush();
  delay(10);
}

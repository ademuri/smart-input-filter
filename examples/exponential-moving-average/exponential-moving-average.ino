#include "exponential-moving-average-filter.h"

const uint8_t kInputPin = 0;
const uint8_t kLed1Pin = 13;
const uint8_t kLed2Pin = 13;
ExponentialMovingAverageFilter<uint32_t> unsignedFilter{
    filter_functions::ForAnalogRead<kInputPin>(), /*alpha=*/128};

ExponentialMovingAverageFilter<int32_t> signedFilter{
    []() { return static_cast<int32_t>(analogRead(kInputPin)) - 512; },
    /*alpha=*/16};

void setup() {
  pinMode(kInputPin, INPUT_PULLUP);
  unsignedFilter.SetLogToSerial(true);
  unsignedFilter.SetMinRunInterval(50);
  // Set an initial value, to avoid filter ramp-up on startup
  unsignedFilter.Initialize(analogRead(kInputPin));

  pinMode(kLed1Pin, OUTPUT);
  pinMode(kLed2Pin, OUTPUT);
}

void loop() {
  unsignedFilter.Run();
  signedFilter.Run();
  digitalWrite(kLed1Pin, unsignedFilter.GetFilteredValue() > 511);
  digitalWrite(kLed2Pin, signedFilter.GetFilteredValue() > 0);
  Serial.flush();
}

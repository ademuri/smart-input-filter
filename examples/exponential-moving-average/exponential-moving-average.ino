#include "exponential-moving-average-filter.h"

const uint8_t kInputPin = 0;
const uint8_t kLedPin = 13;
ExponentialMovingAverageFilter<uint32_t, uint32_t> *input;

void setup() {
  pinMode(kInputPin, INPUT_PULLUP);
  input = new ExponentialMovingAverageFilter<uint32_t, uint32_t>(filter_functions::ForAnalogRead<kInputPin>(), 255);
  input->SetLogToSerial(true);
  input->SetMinRunInterval(50);

  pinMode(kLedPin, OUTPUT);
}

void loop() {
  input->Run();
  digitalWrite(kLedPin, input->GetFilteredValue() > 511);
  Serial.flush();
}

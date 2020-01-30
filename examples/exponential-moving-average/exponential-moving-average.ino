#include "exponential-moving-average-input.h"

const uint8_t kInputPin = 0;
const uint8_t kLedPin = 13;
ExponentialMovingAverageInput *input;

void setup() {
  pinMode(kInputPin, INPUT_PULLUP);
  input = new ExponentialMovingAverageInput(kInputPin, 255);
  input->SetLogToSerial(true);

  pinMode(kLedPin, OUTPUT);
}

void loop() {
  input->Run();
  Serial.flush();
  delay(50);
}

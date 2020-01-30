#include "exponential-moving-average-input.h"

ExponentialMovingAverageInput::ExponentialMovingAverageInput(uint32_t pin, uint8_t alpha) : AnalogFilter(pin), alpha(alpha) {}

uint32_t ExponentialMovingAverageInput::GetFilteredValue() {
  return average;
}

void ExponentialMovingAverageInput::DoRun() {
  uint32_t old_average = average;
  average = (sensor_value * (alpha + 1) + (average * (255 - alpha))) / 256;
  if (old_average == average && sensor_value != average) {
    if (sensor_value > average) {
      average++;
    } else if (sensor_value < average) {
      average--;
    }
  }
}

void ExponentialMovingAverageInput::LogState() {
  Serial.print(sensor_value);
  Serial.print(" ");
  Serial.println(average);
}

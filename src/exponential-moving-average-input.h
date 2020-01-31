#ifndef SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT
#define SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT

#include "analog-filter.h"

template <typename O>
class ExponentialMovingAverageInput : public AnalogFilter<O> {
  using Filter<uint32_t, O>::sensor_value;

 public:
  ExponentialMovingAverageInput(uint32_t pin, uint8_t alpha);
  ExponentialMovingAverageInput(uint32_t pin, uint8_t alpha,
                                O (*Convert)(uint32_t input));

 protected:
  uint32_t DoRun() override;

  void LogState() override;

 private:
  uint32_t average = 0;

  const uint8_t alpha;
};

template <typename O>
ExponentialMovingAverageInput<O>::ExponentialMovingAverageInput(uint32_t pin,
                                                                uint8_t alpha)
    : AnalogFilter<O>(pin), alpha(alpha) {}

template <typename O>
ExponentialMovingAverageInput<O>::ExponentialMovingAverageInput(
    uint32_t pin, uint8_t alpha, O (*Convert)(uint32_t input))
    : AnalogFilter<O>(pin, Convert), alpha(alpha) {}

template <typename O>
uint32_t ExponentialMovingAverageInput<O>::DoRun() {
  uint32_t old_average = average;
  average = (sensor_value * (alpha + 1) + (average * (255 - alpha))) / 256;
  if (old_average == average && sensor_value != average) {
    if (sensor_value > average) {
      average++;
    } else if (sensor_value < average) {
      average--;
    }
  }
  return average;
}

template <typename O>
void ExponentialMovingAverageInput<O>::LogState() {
  Serial.print(sensor_value);
  Serial.print(" ");
  Serial.println(average);
}

#endif

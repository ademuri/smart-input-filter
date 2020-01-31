#ifndef SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT
#define SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT

#include "analog-filter.h"

template <typename OutputType>
class ExponentialMovingAverageInput : public AnalogFilter<OutputType> {
  using Filter<uint32_t, OutputType>::sensor_value_;

 public:
  ExponentialMovingAverageInput(uint32_t pin, uint8_t alpha);
  ExponentialMovingAverageInput(uint32_t pin, uint8_t alpha,
                                OutputType (*Convert)(uint32_t input));

 protected:
  uint32_t DoRun() override;

  void LogState() override;

 private:
  uint32_t average = 0;

  const uint8_t alpha;
};

template <typename OutputType>
ExponentialMovingAverageInput<OutputType>::ExponentialMovingAverageInput(
    uint32_t pin, uint8_t alpha)
    : AnalogFilter<OutputType>(pin), alpha(alpha) {}

template <typename OutputType>
ExponentialMovingAverageInput<OutputType>::ExponentialMovingAverageInput(
    uint32_t pin, uint8_t alpha, OutputType (*Convert)(uint32_t input))
    : AnalogFilter<OutputType>(pin, Convert), alpha(alpha) {}

template <typename OutputType>
uint32_t ExponentialMovingAverageInput<OutputType>::DoRun() {
  uint32_t old_average = average;
  average = (sensor_value_ * (alpha + 1) + (average * (255 - alpha))) / 256;
  if (old_average == average && sensor_value_ != average) {
    if (sensor_value_ > average) {
      average++;
    } else if (sensor_value_ < average) {
      average--;
    }
  }
  return average;
}

template <typename OutputType>
void ExponentialMovingAverageInput<OutputType>::LogState() {
  Serial.print(sensor_value_);
  Serial.print(" ");
  Serial.println(average);
}

#endif

#ifndef SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT
#define SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT

#include "analog-filter.h"

// An exponential moving average filter. This uses only integer (32-bit) math.
// This supports up to 24-bit inputs.
//
// An exponential moving average filter is defined as:
//    average = input_value * alpha + previous_average * (1 - alpha)
// This filter takes an alpha from 0 to 255, with 255 corresponding to 1 on a
// typical floating-point based filter.
//
// An alpha of 255 means that the filter returns the current value of the input.
// An alpha of 0 means the filtered value changes very slowly.
template <typename OutputType>
class ExponentialMovingAverageFilter : public AnalogFilter<OutputType> {
  using Filter<uint32_t, OutputType>::sensor_value_;

 public:
  ExponentialMovingAverageFilter(uint32_t pin, uint8_t alpha);
  ExponentialMovingAverageFilter(uint32_t pin, uint8_t alpha,
                                 OutputType (*Convert)(uint32_t input));

 protected:
  uint32_t DoRun() override;

 private:
  uint32_t average_ = 0;

  const uint8_t alpha_;
};

template <typename OutputType>
ExponentialMovingAverageFilter<OutputType>::ExponentialMovingAverageFilter(
    uint32_t pin, uint8_t alpha)
    : AnalogFilter<OutputType>(pin), alpha_(alpha) {}

template <typename OutputType>
ExponentialMovingAverageFilter<OutputType>::ExponentialMovingAverageFilter(
    uint32_t pin, uint8_t alpha, OutputType (*Convert)(uint32_t input))
    : AnalogFilter<OutputType>(pin, Convert), alpha_(alpha) {}

template <typename OutputType>
uint32_t ExponentialMovingAverageFilter<OutputType>::DoRun() {
  uint32_t old_average = average_;
  average_ = (sensor_value_ * (alpha_ + 1) + (average_ * (255 - alpha_))) / 256;
  if (old_average == average_ && sensor_value_ != average_) {
    if (sensor_value_ > average_) {
      average_++;
    } else if (sensor_value_ < average_) {
      average_--;
    }
  }
  return average_;
}

#endif

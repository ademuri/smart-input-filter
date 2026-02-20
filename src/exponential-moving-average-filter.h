#ifndef SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT
#define SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT

#include "filter.h"

// An exponential moving average filter. This uses only integer math.
// This uses 64-bit integers internally, so it supports up to 55-bit inputs.
//
// An exponential moving average filter is defined as:
//    average = input_value * alpha + previous_average * (1 - alpha)
// This filter takes an alpha from 0 to 255, with 255 corresponding to 1 on a
// typical floating-point based filter.
//
// An alpha of 255 means that the filter returns the current value of the input.
// An alpha of 0 means the filtered value changes very slowly.
template <typename InputType = int32_t, typename OutputType = InputType>
class ExponentialMovingAverageFilter : public Filter<InputType, OutputType> {
  using Filter<InputType, OutputType>::sensor_value_;

 public:
  ExponentialMovingAverageFilter(
      typename Filter<InputType, OutputType>::ReadFromSensorType ReadFromSensor,
      uint8_t alpha);
  ExponentialMovingAverageFilter(
      typename Filter<InputType, OutputType>::ReadFromSensorType ReadFromSensor,
      uint8_t alpha, OutputType (*Convert)(InputType input));

  void Initialize(InputType average);

 protected:
  InputType DoRun() override;

 private:
  InputType average_ = 0;

  const uint8_t alpha_;
};

template <typename InputType, typename OutputType>
ExponentialMovingAverageFilter<InputType, OutputType>::
    ExponentialMovingAverageFilter(
        typename Filter<InputType, OutputType>::ReadFromSensorType
            ReadFromSensor,
        uint8_t alpha)
    : Filter<InputType, OutputType>(ReadFromSensor), alpha_(alpha) {}

template <typename InputType, typename OutputType>
ExponentialMovingAverageFilter<InputType, OutputType>::
    ExponentialMovingAverageFilter(
        typename Filter<InputType, OutputType>::ReadFromSensorType
            ReadFromSensor,
        uint8_t alpha, OutputType (*Convert)(InputType input))
    : Filter<InputType, OutputType>(ReadFromSensor, Convert), alpha_(alpha) {}

template <typename InputType, typename OutputType>
InputType ExponentialMovingAverageFilter<InputType, OutputType>::DoRun() {
  const InputType old_average = average_;
  // Use 64-bit math for the intermediate sum to prevent overflow.
  average_ = (static_cast<int64_t>(sensor_value_) * (alpha_ + 1) +
              (static_cast<int64_t>(average_) * (255 - alpha_))) /
             256;
  if (old_average == average_ && sensor_value_ != average_) {
    if (sensor_value_ > average_) {
      average_++;
    } else if (sensor_value_ < average_) {
      average_--;
    }
  }
  return average_;
}

template <typename InputType, typename OutputType>
void ExponentialMovingAverageFilter<InputType, OutputType>::Initialize(
    InputType average) {
  average_ = average;
}

#endif

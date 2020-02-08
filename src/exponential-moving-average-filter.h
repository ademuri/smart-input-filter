#ifndef SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT
#define SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT

#include "filter.h"

#include <type_traits>

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
template <typename InputType, typename OutputType>
class ExponentialMovingAverageFilter : public Filter<InputType, OutputType> {
  using Filter<InputType, OutputType>::sensor_value_;

 public:
  ExponentialMovingAverageFilter(InputType (*const ReadFromSensor)(),
                                 InputType alpha);
  ExponentialMovingAverageFilter(InputType (*const ReadFromSensor)(),
                                 InputType alpha,
                                 OutputType (*Convert)(InputType input));

 protected:
  InputType DoRun() override;

 private:
  InputType average_ = 0;

  const InputType alpha_;
};

template <typename InputType, typename OutputType>
ExponentialMovingAverageFilter<InputType, OutputType>::
    ExponentialMovingAverageFilter(InputType (*const ReadFromSensor)(),
                                   InputType alpha)
    : Filter<InputType, OutputType>(ReadFromSensor), alpha_(alpha) {}

template <typename InputType, typename OutputType>
ExponentialMovingAverageFilter<InputType, OutputType>::
    ExponentialMovingAverageFilter(InputType (*const ReadFromSensor)(),
                                   InputType alpha,
                                   OutputType (*Convert)(InputType input))
    : Filter<InputType, OutputType>(ReadFromSensor, Convert), alpha_(alpha) {}

template <typename InputType, typename OutputType>
InputType ExponentialMovingAverageFilter<InputType, OutputType>::DoRun() {
  InputType old_average = average_;
  if (std::is_integral<InputType>::value) {
    average_ =
        (sensor_value_ * (alpha_ + 1) + (average_ * (255 - alpha_))) / 256;
    if (old_average == average_ && sensor_value_ != average_) {
      if (sensor_value_ > average_) {
        average_++;
      } else if (sensor_value_ < average_) {
        average_--;
      }
    }
  } else {
    average_ = (sensor_value_ * (alpha_) + (average_ * (1.0 - alpha_)));
  }
  return average_;
}

#endif

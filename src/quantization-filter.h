#pragma once

#include "filter.h"

template <typename InputType>
class QuantizationFilter : public Filter<InputType, InputType> {
  using Filter<InputType, InputType>::sensor_value_;

 public:
  QuantizationFilter(typename Filter<InputType, InputType>::ReadFromSensorType
                         read_from_sensor,
                     InputType bucket_size, InputType hysteresis);

 protected:
  InputType DoRun() override;

 private:
  InputType prev_value_ = 0;
  InputType prev_quantized_value_ = 0;

  const InputType bucket_size_;
  const InputType hysteresis_;
};

template <typename InputType>
QuantizationFilter<InputType>::QuantizationFilter(
    typename Filter<InputType, InputType>::ReadFromSensorType read_from_sensor,
    InputType bucket_size, InputType hysteresis)
    : Filter<InputType, InputType>(read_from_sensor),
      bucket_size_(bucket_size),
      hysteresis_(hysteresis) {}

template <typename InputType>
InputType QuantizationFilter<InputType>::DoRun() {
  const InputType new_quantized_value =
      (sensor_value_ / bucket_size_) * bucket_size_;
  const InputType difference = sensor_value_ > prev_quantized_value_
                                   ? sensor_value_ - prev_quantized_value_
                                   : prev_quantized_value_ - sensor_value_;
  if (difference >= (bucket_size_ + hysteresis_)) {
    prev_quantized_value_ = new_quantized_value;
  }

  prev_value_ = sensor_value_;
  return prev_quantized_value_;
}

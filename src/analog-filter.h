#ifndef SMART_INPUT_FILTER_ANALOG_FILTER
#define SMART_INPUT_FILTER_ANALOG_FILTER

#include "filter.h"

template <typename OutputType>
class AnalogFilter : public Filter<uint32_t, OutputType> {
#ifndef ARDUINO
  using Filter<uint32_t, OutputType>::pin_value_;
#endif

 public:
  AnalogFilter(uint32_t pin);
  AnalogFilter(uint32_t pin, OutputType (*Convert)(uint32_t input));

 protected:
  uint32_t ReadFromSensor() override;

 private:
  const uint32_t pin;

#ifndef ARDUINO
  uint32_t analogRead(uint32_t pin);
#endif
};

template <typename OutputType>
AnalogFilter<OutputType>::AnalogFilter(uint32_t pin) : pin(pin) {}

template <typename OutputType>
AnalogFilter<OutputType>::AnalogFilter(uint32_t pin,
                                       OutputType (*Convert)(uint32_t input))
    : Filter<uint32_t, OutputType>(Convert), pin(pin) {}

template <typename OutputType>
uint32_t AnalogFilter<OutputType>::ReadFromSensor() {
  return analogRead(pin);
}

#ifndef ARDUINO
template <typename OutputType>
uint32_t AnalogFilter<OutputType>::analogRead(uint32_t pin) {
  (void)pin;
  return pin_value_;
}
#endif

#endif

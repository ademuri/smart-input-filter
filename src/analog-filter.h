#ifndef SMART_INPUT_FILTER_ANALOG_FILTER
#define SMART_INPUT_FILTER_ANALOG_FILTER

#include "filter.h"

template <typename O>
class AnalogFilter : public Filter<uint32_t, O> {
  using Filter<uint32_t, O>::pin_value;

 public:
  AnalogFilter(uint32_t pin);

 protected:
  uint32_t ReadFromSensor() override;

 private:
  const uint32_t pin;

#ifndef ARDUINO
  uint32_t analogRead(uint32_t pin);
#endif
};

template <typename O>
AnalogFilter<O>::AnalogFilter(uint32_t pin) : pin(pin) {}

template <typename O>
uint32_t AnalogFilter<O>::ReadFromSensor() {
  return analogRead(pin);
}

#ifndef ARDUINO
template <typename O>
uint32_t AnalogFilter<O>::analogRead(uint32_t pin) {
  (void)pin;
  return pin_value;
}
#endif

#endif

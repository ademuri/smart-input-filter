#ifndef SMART_INPUT_FILTER_ANALOG_FILTER
#define SMART_INPUT_FILTER_ANALOG_FILTER

#include "filter.h"

class AnalogFilter : public Filter<uint32_t> {
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

#endif

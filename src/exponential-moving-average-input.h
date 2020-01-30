#ifndef SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT
#define SMART_INPUT_FILTER_EXPONENTIAL_MOVING_AVERAGE_INPUT

#include "analog-filter.h"

class ExponentialMovingAverageInput : public AnalogFilter {
 public:
  ExponentialMovingAverageInput(uint32_t pin, uint8_t alpha);
  uint32_t GetFilteredValue();

 protected:
  void DoRun() override;

  void LogState() override;

 private:
  uint32_t average = 0;

  const uint8_t alpha;
};

#endif

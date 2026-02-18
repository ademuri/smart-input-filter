#include "blink-filter.h"

BlinkFilter::BlinkFilter(
    typename Filter<bool, bool>::ReadFromSensorType ReadFromSensor,
    uint32_t expectedPeriodMs, uint8_t toleranceFraction,
    uint16_t lookbackPeriods)
    : Filter(ReadFromSensor),
      expected_period_ms_(expectedPeriodMs),
      tolerance_fraction_(toleranceFraction),
      lookback_periods_(lookbackPeriods) {
  Reset();
}

bool BlinkFilter::IsBlinking() const {
  return is_blinking_;
}

void BlinkFilter::Reset() {
  is_blinking_ = false;
  consecutive_half_cycles_ = 0;
  last_transition_time_ms_ = 0;
  last_input_state_ = false;
}

bool BlinkFilter::DoRun() {
  // To be implemented in Phase 3
  return sensor_value_;
}

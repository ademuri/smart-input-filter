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

void BlinkFilter::Reset() {
  Filter<bool, bool>::Reset();
  is_blinking_ = false;
  first_run_ = true;
  has_start_time_ = false;
  consecutive_half_cycles_ = 0;
  last_transition_time_ms_ = 0;
  last_input_state_ = false;
}

bool BlinkFilter::DoRun() {
  uint32_t current_time = millis();
  bool current_input = sensor_value_;

  if (first_run_) {
    last_input_state_ = current_input;
    first_run_ = false;
    return is_blinking_;
  }

  uint32_t expected_half_period = expected_period_ms_ / 2;
  uint32_t tolerance = (expected_half_period * tolerance_fraction_) / 255;

  if (current_input != last_input_state_) {
    if (has_start_time_) {
      uint32_t duration = current_time - last_transition_time_ms_;

      if (duration >= (expected_half_period - tolerance) &&
          duration <= (expected_half_period + tolerance)) {
        consecutive_half_cycles_++;
        if (consecutive_half_cycles_ >= lookback_periods_) {
          is_blinking_ = true;
        }
      } else {
        consecutive_half_cycles_ = 0;
        is_blinking_ = false;
      }
    }
    last_transition_time_ms_ = current_time;
    has_start_time_ = true;
    last_input_state_ = current_input;
  } else {
    // Check for timeout (too long since last transition)
    if (has_start_time_) {
      uint32_t current_duration = current_time - last_transition_time_ms_;
      if (current_duration > (expected_half_period + tolerance)) {
        consecutive_half_cycles_ = 0;
        is_blinking_ = false;
        has_start_time_ = false;
      }
    }
  }

  return is_blinking_;
}

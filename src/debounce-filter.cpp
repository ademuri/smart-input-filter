#include "debounce-filter.h"

DebounceFilter::DebounceFilter(bool (*const ReadFromSensor)()) : Filter(ReadFromSensor) {}

bool DebounceFilter::Rose() { return rose_; }

bool DebounceFilter::Fell() { return fell_; }

bool DebounceFilter::DoRun() {
  rose_ = false;
  fell_ = false;

  if (millis() - state_started_at_millis >= kDebounceTimeMillis) {
    if (stable_state_ != current_state_) {
      stable_state_ = current_state_;
      last_successful_change_at_millis = millis();

      if (stable_state_) {
        rose_ = true;
      } else {
        fell_ = true;
      }
    }
  }

  if (sensor_value_ == current_state_) {
    return stable_state_;
  }

  if (millis() - last_successful_change_at_millis >= kDebounceTimeMillis) {
    if (stable_state_ != sensor_value_) {
      if (sensor_value_) {
        rose_ = true;
      } else {
        fell_ = true;
      }
    }

    stable_state_ = sensor_value_;
    current_state_ = sensor_value_;
    last_successful_change_at_millis = millis();
    state_started_at_millis = millis();
  }

  // Input != current_state_
  state_started_at_millis = millis();
  current_state_ = sensor_value_;
  return stable_state_;
}


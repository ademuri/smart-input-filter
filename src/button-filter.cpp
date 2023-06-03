#include "button-filter.h"

ButtonFilter::ButtonFilter(
    typename Filter<bool, bool>::ReadFromSensorType ReadFromSensor,
    uint16_t held_time_millis)
    : DebounceFilter(ReadFromSensor), held_time_millis_(held_time_millis) {}

bool ButtonFilter::Pressed() { return pressed_; }

bool ButtonFilter::Held() { return held_; }

bool ButtonFilter::DoRun() {
  bool ret = DebounceFilter::DoRun();

  pressed_ = false;
  held_ = false;
  repeated_ = false;

  if (Rose()) {
    press_started_millis_ = millis();
    held_this_press_ = false;
  } else if (Fell()) {
    if (!held_this_press_) {
      pressed_ = true;
    }
    press_started_millis_ = 0;
    next_repeat_at_ = 0;
  } else if (next_repeat_at_ != 0 && millis() > next_repeat_at_) {
    repeated_ = true;
    next_repeat_at_ = millis() + repeat_delay_ms_;
  } else if (!held_this_press_ && press_started_millis_ != 0 &&
             (millis() - press_started_millis_) >= held_time_millis_) {
    held_this_press_ = true;
    held_ = true;
    if (repeat_delay_ms_ != 0) {
      next_repeat_at_ = millis() + repeat_delay_ms_;
    }
  }

  return ret;
}

void ButtonFilter::SetRepeatDelay(uint32_t repeat_delay_ms) {
  if (repeat_delay_ms == 0) {
    next_repeat_at_ = 0;
  } else if (next_repeat_at_ != 0) {
    next_repeat_at_ = next_repeat_at_ - repeat_delay_ms_ + repeat_delay_ms;
  }
  repeat_delay_ms_ = repeat_delay_ms;
}

bool ButtonFilter::Repeated() { return repeated_; }

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

  if (Rose()) {
    press_started_millis_ = millis();
  } else if (Fell()) {
    if ((millis() - press_started_millis_) >= held_time_millis_) {
      held_ = true;
    } else {
      pressed_ = true;
    }
    press_started_millis_ = 0;
  }

  return ret;
}

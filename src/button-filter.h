#pragma once

#include "debounce-filter.h"
#include "filter.h"

class ButtonFilter : public DebounceFilter {
 public:
  ButtonFilter(typename Filter<bool, bool>::ReadFromSensorType ReadFromSensor,
               uint16_t held_time_millis);

  // Whether the button was pressed this cycle. Rest on the next call to Run.
  // True once per button press cycle.
  bool Pressed();

  // Whether the button was held this cycle. Rest on the next call to Run. True
  // once per button press cycle, i.e. the button must be released and then held
  // again before this will be true again.
  bool Held();

 protected:
  bool DoRun() override;

 private:
  const uint16_t held_time_millis_;

  uint32_t press_started_millis_ = 0;
  bool pressed_ = false;
  bool held_ = false;
};
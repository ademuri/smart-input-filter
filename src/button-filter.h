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

  // Whether the button was held this cycle. Reset on the next call to Run. True
  // once per button press cycle, i.e. the button must be released and then held
  // again before this will be true again.
  bool Held();

  // Sets the interval at which Repeated() will return true. 0 to disable.
  void SetRepeatDelay(uint32_t repeat_delay_ms);

  // When repeat_delay is non-zero, this will return true while the button is
  // held every `repeat_delay_ms` milliseconds after Held() returns true.
  bool Repeated();

 protected:
  bool DoRun() override;

 private:
  const uint16_t held_time_millis_;

  uint32_t press_started_millis_ = 0;
  uint32_t repeat_delay_ms_ = 0;
  uint32_t next_repeat_at_ = 0;
  bool pressed_ = false;
  bool held_ = false;
  bool repeated_ = false;
  bool held_this_press_ = false;
};
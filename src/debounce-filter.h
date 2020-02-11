#ifndef SMART_INPUT_FILTER_DEBOUNCE_INPUT
#define SMART_INPUT_FILTER_DEBOUNCE_INPUT

#include "filter.h"

// Debounces an input such as a switch. Responds immediately if the input
// hasn't changed in a while (so that it catches the initial switch with no
// latency).
class DebounceFilter : public Filter<bool, bool> {
 public:
  DebounceFilter(
      typename Filter<bool, bool>::ReadFromSensorType ReadFromSensor);

  // Whether the input rose this cycle. Reset on the next call to Run.
  bool Rose();

  // Whether the input fell this cycle. Reset on the next call to Run.
  bool Fell();

 protected:
  bool DoRun() override;

 private:
  bool stable_state_ = false;
  bool current_state_ = false;
  bool rose_ = false;
  bool fell_ = false;
  uint32_t state_started_at_millis = 0;
  uint32_t last_successful_change_at_millis = 0;

  const uint16_t kDebounceTimeMillis = 10;
};

#endif

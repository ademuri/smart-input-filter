#ifndef SMART_INPUT_FILTER_DEBOUNCE_INPUT
#define SMART_INPUT_FILTER_DEBOUNCE_INPUT

#include "filter.h"

// Debounces an input such as a switch. Responds immediately if the input
// hasn't changed in a while (so that it catches the initial switch with no
// latency).
class DebounceInput : public Filter<bool, bool> {
 public:
  DebounceInput(uint32_t pin);

  // Whether the input rose this cycle. Reset on the next call to Run.
  bool Rose();

  // Whether the input fell this cycle. Reset on the next call to Run.
  bool Fell();

 protected:
  bool DoRun() override;
  bool ReadFromSensor() override;

 private:
  bool stable_state = false;
  bool current_state = false;
  bool rose = false;
  bool fell = false;
  uint32_t state_started_at_millis = 0;
  uint32_t last_successful_change_at_millis = 0;

  const uint32_t pin;
  const uint16_t kDebounceTimeMillis = 10;

#ifndef ARDUINO
  int digitalRead(uint32_t pin);
#endif
};

#endif

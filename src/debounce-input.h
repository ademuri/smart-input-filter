#ifndef ARDUINO_INPUT_FILTER_DEBOUNCE_INPUT
#define ARDUINO_INPUT_FILTER_DEBOUNCE_INPUT

#include "filter.h"

/*
 * Debounces an input such as a switch. Responds immediately if the input
 * hasn't changed in a while (so that it catches the initial switch with no
 * latency).
 */
class DebounceInput : public Filter<int> {
 public:
  DebounceInput(uint32_t pin);

  /* Gets the current debounced state. */
  bool GetCurrentState();

  /* Whether the input rose this cycle. Reset on the next call to Run. */
  bool Rose();

  /* Whether the input fell this cycle. Reset on the next call to Run. */
  bool Fell();

#ifndef ARDUINO
  void SetPinValue(int value);
#endif

 protected:
  void DoRun() override;
  int GetRawValue() override;

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
  int pin_value = 0;
  int digitalRead(uint32_t pin);
#endif
};

#endif

#ifndef ARDUINO_INPUT_FILTER_DEBOUNCE_INPUT
#define ARDUINO_INPUT_FILTER_DEBOUNCE_INPUT

#include "filter.h"

class DebounceInput : public Filter {
  public:
    DebounceInput(uint16_t pin);

    bool GetCurrentState();

  protected:
    void DoRun() override;

  private:
    bool stable_state = false;
    bool current_state = false;
    uint32_t state_started_at_millis = 0;

    const uint16_t pin;
    const uint16_t kDebounceTimeMillis = 10;
};

#endif

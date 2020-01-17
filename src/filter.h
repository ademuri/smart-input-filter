#ifndef ARDUINO_INPUT_FILTER_FILTER
#define ARDUINO_INPUT_FILTER_FILTER

#ifndef ARDUINO
#include <cstdint>
#endif

class Filter {
  public:
    void Run();

  protected:
    virtual void DoRun() = 0;

    void SetRunDelayInMillis(uint32_t delay);

#ifndef ARDUINO
    uint32_t millis();

    uint8_t digitalRead(uint8_t pin);
#endif

  private:
    uint32_t run_at = 0;
};

#endif

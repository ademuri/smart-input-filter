#ifndef ARDUINO_INPUT_FILTER_FILTER
#define ARDUINO_INPUT_FILTER_FILTER

#ifndef ARDUINO
#include <cstdint>
#include <vector>
#endif

class Filter {
  public:
    void Run();

#ifndef ARDUINO
    void SetMillis(uint32_t value);
    
    void SetPin(uint8_t pin, uint8_t value);
#endif

  protected:
    virtual void DoRun() = 0;

    void SetRunDelayInMillis(uint32_t delay);

#ifndef ARDUINO
    uint32_t millis();

    uint8_t digitalRead(uint8_t pin);
#endif

  private:
    uint32_t run_at = 0;

#ifndef ARDUINO
    uint32_t fake_millis = 0;

    std::vector<uint8_t> pins = std::vector<uint8_t>(255, 0);
#endif
};

#endif

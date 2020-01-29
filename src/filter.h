#ifndef ARDUINO_INPUT_FILTER_FILTER
#define ARDUINO_INPUT_FILTER_FILTER

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <vector>
#endif

/* T is the input type (e.g. int for digitalRead, uint32_t for analogRead) */
template <typename T>
class Filter {
 public:
  /* Run one iteration of the filter. Call this periodically to read the sensor
   * and run the filtering logic. */
  void Run();

#ifndef ARDUINO
  void SetMillis(uint32_t value);
#endif

 protected:
  void SetRunDelayInMillis(uint32_t delay);

  /* Perform the filtering logic. Override this. */
  virtual void DoRun() = 0;

  /* Gets the raw value of the sensor. Override this to hook up to a physical
   * sensor. */
  virtual T GetRawValue() = 0;

#ifndef ARDUINO
  uint32_t millis();
#endif

 private:
  uint32_t run_at = 0;

#ifndef ARDUINO
  uint32_t fake_millis = 0;
#endif
};

template <class T>
void Filter<T>::Run() {
  if (millis() >= run_at) {
    DoRun();
  }
}

template <class T>
void Filter<T>::SetRunDelayInMillis(uint32_t delay) {
  run_at = millis() + delay;
}

#ifndef ARDUINO
template <class T>
uint32_t Filter<T>::millis() {
  return fake_millis;
}

template <class T>
void Filter<T>::SetMillis(uint32_t value) {
  fake_millis = value;
}
#endif

#endif

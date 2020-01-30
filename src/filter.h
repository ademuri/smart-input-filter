#ifndef SMART_INPUT_FILTER_FILTER
#define SMART_INPUT_FILTER_FILTER

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <iostream>
#include <vector>
#endif

/* I is the input type (e.g. int for digitalRead, uint32_t for analogRead) */
template <typename I>
class Filter {
 public:
  /* Run one iteration of the filter. Call this periodically to read the sensor
   * and run the filtering logic. */
  void Run();

  void SetLogToSerial(bool log);

#ifndef ARDUINO
  void SetMillis(uint32_t value);

  void SetPinValue(I value);
#endif

 protected:
  void SetRunDelayInMillis(uint32_t delay);

  /* Perform the filtering logic. Override this. */
  virtual void DoRun() = 0;

  /* Gets the raw value of the sensor. Override this to hook up to a physical
   * sensor. */
  virtual I ReadFromSensor() = 0;

  /* Prints debugging information to the serial console (e.g. unfiltered and
   * filtered value). Override this. */
  virtual void LogState() = 0;

  /* Current cached value of the sensor. Set in Run. Used so that each
   * iteration of Run only reads from the sensor once. */
  I sensor_value = 0;

#ifndef ARDUINO
  uint32_t millis();

  I pin_value = 0;
#endif

 private:
  uint32_t run_at = 0;
  bool log_to_serial = false;

#ifndef ARDUINO
  uint32_t fake_millis = 0;
#endif
};

template <class I>
void Filter<I>::Run() {
  if (millis() >= run_at) {
    sensor_value = ReadFromSensor();
    DoRun();
    if (log_to_serial) {
      LogState();
    }
  }
}

template <class I>
void Filter<I>::SetLogToSerial(bool log) {
  log_to_serial = log;
}

template <class I>
void Filter<I>::SetRunDelayInMillis(uint32_t delay) {
  run_at = millis() + delay;
}

#ifndef ARDUINO
template <class I>
void Filter<I>::SetMillis(uint32_t value) {
  fake_millis = value;
}

template <class I>
void Filter<I>::SetPinValue(I value) {
  pin_value = value;
}

template <class I>
uint32_t Filter<I>::millis() {
  return fake_millis;
}

class SerialClass {
 public:
  template <typename T>
  void print(T arg) {
    std::cout << arg;
  }

  template <typename T>
  void println(T arg) {
    std::cout << arg << "\n";
  }
};
extern SerialClass Serial;

#endif

#endif

#ifndef SMART_INPUT_FILTER_FILTER
#define SMART_INPUT_FILTER_FILTER

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <iostream>
#include <vector>
#endif

/* I is the input type (e.g. int for digitalRead, uint32_t for analogRead). O is
 * the output type. The filter converts from the input type to the output type
 * (with the default being no conversion). */
template <typename I, typename O>
class Filter {
 public:
  /* Run one iteration of the filter. Call this periodically to read the sensor
   * and run the filtering logic. */
  void Run();

  void SetLogToSerial(bool log);

  I GetFilteredValue();

#ifndef ARDUINO
  void SetMillis(uint32_t value);

  void SetPinValue(I value);
#endif

 protected:
  void SetRunDelayInMillis(uint32_t delay);

  /* Perform the filtering logic. Override this. */
  virtual I DoRun() = 0;

  /* Gets the raw value of the sensor. Override this to hook up to a physical
   * sensor. */
  virtual I ReadFromSensor() = 0;

  /* Prints debugging information to the serial console (e.g. unfiltered and
   * filtered value). Override this. */
  virtual void LogState();

  O (*Convert)(I input);

  /* Current cached value of the sensor. Set in Run. Used so that each
   * iteration of Run only reads from the sensor once. */
  I sensor_value = 0;

#ifndef ARDUINO
  uint32_t millis();

  I pin_value = 0;
#endif

 private:
  I filtered_value = 0;
  uint32_t run_at = 0;
  bool log_to_serial = false;

#ifndef ARDUINO
  uint32_t fake_millis = 0;
#endif
};

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

template <typename I, typename O>
void Filter<I, O>::Run() {
  if (millis() >= run_at) {
    sensor_value = ReadFromSensor();
    filtered_value = DoRun();
    if (log_to_serial) {
      LogState();
    }
  }
}

template <typename I, typename O>
void Filter<I, O>::SetLogToSerial(bool log) {
  log_to_serial = log;
}

template <typename I, typename O>
I Filter<I, O>::GetFilteredValue() {
  return filtered_value;
}

template <typename I, typename O>
void Filter<I, O>::SetRunDelayInMillis(uint32_t delay) {
  run_at = millis() + delay;
}

template <typename I, typename O>
void Filter<I, O>::LogState() {
  Serial.print(sensor_value);
  Serial.print(" ");
  Serial.println(filtered_value);
}

#ifndef ARDUINO
template <typename I, typename O>
void Filter<I, O>::SetMillis(uint32_t value) {
  fake_millis = value;
}

template <typename I, typename O>
void Filter<I, O>::SetPinValue(I value) {
  pin_value = value;
}

template <typename I, typename O>
uint32_t Filter<I, O>::millis() {
  return fake_millis;
}

#endif

#endif

#ifndef SMART_INPUT_FILTER_FILTER
#define SMART_INPUT_FILTER_FILTER

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint>
#include <iostream>
#include <vector>
#endif

// InputType is the input type (e.g. int for digitalRead, uint32_t for
// analogRead). OutputType is the output type. The filter converts from the
// input type to the output type (with the default being no conversion).
template <typename InputType, typename OutputType>
class Filter {
 public:
  Filter() : Convert_(Filter<InputType, OutputType>::NoOpConvert) {}

  Filter(OutputType (*Convert)(InputType input)) : Convert_(Convert) {}

  // Run one iteration of the filter. Call this periodically to read the sensor
  // and run the filtering logic.
  void Run();

  void SetLogToSerial(bool log);

  OutputType GetFilteredValue();

#ifndef ARDUINO
  void SetMillis(uint32_t value);

  void SetPinValue(InputType value);
#endif

 protected:
  void SetRunDelayInMillis(uint32_t delay);

  // Perform the filtering logic. Override this.
  virtual InputType DoRun() = 0;

  // Gets the raw value of the sensor. Override this to hook up to a physical
  // sensor.
  virtual InputType ReadFromSensor() = 0;

  // Prints debugging information to the serial console (e.g. unfiltered and
  // filtered value). Override this.
  virtual void LogState();

  OutputType (*const Convert_)(InputType input);

  // Current cached value of the sensor. Set in Run. Used so that each
  // iteration of Run only reads from the sensor once.
  InputType sensor_value_ = 0;

#ifndef ARDUINO
  uint32_t millis();

  InputType pin_value_ = 0;
#endif

 private:
  static OutputType NoOpConvert(InputType input);

  InputType filtered_value_ = 0;
  uint32_t run_at_ = 0;
  bool log_to_serial_ = false;

#ifndef ARDUINO
  uint32_t fake_millis_ = 0;
#endif
};

#ifndef ARDUINO
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

template <typename InputType, typename OutputType>
void Filter<InputType, OutputType>::Run() {
  if (millis() >= run_at_) {
    sensor_value_ = ReadFromSensor();
    filtered_value_ = DoRun();
    if (log_to_serial_) {
      LogState();
    }
  }
}

template <typename InputType, typename OutputType>
void Filter<InputType, OutputType>::SetLogToSerial(bool log) {
  log_to_serial_ = log;
}

template <typename InputType, typename OutputType>
OutputType Filter<InputType, OutputType>::GetFilteredValue() {
  return Convert_(filtered_value_);
}

template <typename InputType, typename OutputType>
void Filter<InputType, OutputType>::SetRunDelayInMillis(uint32_t delay) {
  run_at_ = millis() + delay;
}

template <typename InputType, typename OutputType>
void Filter<InputType, OutputType>::LogState() {
  Serial.print(Convert_(sensor_value_));
  Serial.print(" ");
  Serial.println(Convert_(filtered_value_));
}

#ifndef ARDUINO
template <typename InputType, typename OutputType>
void Filter<InputType, OutputType>::SetMillis(uint32_t value) {
  fake_millis_ = value;
}

template <typename InputType, typename OutputType>
void Filter<InputType, OutputType>::SetPinValue(InputType value) {
  pin_value_ = value;
}

template <typename InputType, typename OutputType>
uint32_t Filter<InputType, OutputType>::millis() {
  return fake_millis_;
}

#endif

template <typename InputType, typename OutputType>
OutputType Filter<InputType, OutputType>::NoOpConvert(InputType input) {
  return (OutputType)input;
}

#endif

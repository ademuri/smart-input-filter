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
  // Default constructor uses a no-op converter.
  Filter(InputType (*const ReadFromSensor)())
      : ReadFromSensor_(ReadFromSensor),
        Convert_(Filter<InputType, OutputType>::NoOpConvert) {}

  Filter(InputType (*const ReadFromSensor)(),
         OutputType (*Convert)(InputType input))
      : ReadFromSensor_(ReadFromSensor), Convert_(Convert) {}

  // Run one iteration of the filter. Call this periodically to read the sensor
  // and run the filtering logic.
  void Run();

  // Returns the output of the filter, converted.
  OutputType GetFilteredValue();

  // Controls whether to log the input and output values of this filter. Values
  // are logged to Serial in the form <input> <output>. Both input and output
  // are converted before logging. These values can be graphed using the
  // Arduino serial plotter.
  void SetLogToSerial(bool log);

  // Used to construct input function for standard Arduino inputs
  template <uint32_t pin>
  static bool (*const ForDigitalRead)();

  // Setters for tests
#ifndef ARDUINO
  void SetMillis(uint32_t value);

  void SetPinValue(InputType value);
#endif

 protected:
  // This will instruct the filter to not run until at least delay milliseconds
  // have passed.
  void SetRunDelayInMillis(uint32_t delay);

  // Perform the filtering logic. Override this.
  virtual InputType DoRun() = 0;

  // Gets the raw value of the sensor.
  InputType (*const ReadFromSensor_)();

  // Converts from the sensor type to the usage type (e.g. converting from a
  // 10-bit uint32_t that analogRead returns to a voltage).
  OutputType (*const Convert_)(InputType input);

  // Prints debugging information to the serial console (e.g. unfiltered and
  // filtered value). Override this.
  virtual void LogState();

  // Current cached value of the sensor. Set in Run. Used so that each
  // iteration of Run only reads from the sensor once.
  InputType sensor_value_ = 0;

  // Test functions
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
    sensor_value_ = ReadFromSensor_();
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

template <uint32_t pin>
static bool (*const Filter::ForDigitalRead)() {
  return digitalRead(pin);
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

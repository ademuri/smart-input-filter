#ifndef SMART_INPUT_FILTER_FILTER
#define SMART_INPUT_FILTER_FILTER

#ifdef ARDUINO
#include <Arduino.h>

// Arduino.h, automatically included by the IDE, defines min and max macros
// (which it shouldn't). Apparently FastLED depends on them, so don't undef
// them until FastLED has been included.
#undef max
#undef min

// The AVR gcc compiler doesn't have a lot of STL. On those platforms (e.g. the
// Uno), include the ArduinoSTL library which provides some of that
// functionality. You'll need to install the library  (typically through the
// Arduino library manager).
#ifdef __AVR__
#include <ArduinoSTL.h>
#endif

#else
#include <cstdint>
#include <iostream>
#include <vector>

// Arduino functions
extern int digitalRead(uint32_t pin);
extern uint32_t analogRead(uint32_t pin);
#endif

#include <functional>

// InputType is the input type (e.g. int for digitalRead, uint32_t for
// analogRead). OutputType is the output type. The filter converts from the
// input type to the output type (with the default being no conversion).
template <typename InputType, typename OutputType>
class Filter {
 public:
#ifdef __AVR__
  // avr-gcc doesn't have std::function - fortunately, std::function is
  // "backwards-compatible" with raw function pointers.
  typedef InputType (*const ReadFromSensorType)();
#else
  // On modern platforms that have std::function, this allows callers to e.g.
  // use `bind` here.
  typedef std::function<InputType()> ReadFromSensorType;
#endif

  // Default constructor uses a no-op converter.
  Filter(ReadFromSensorType ReadFromSensor)
      : ReadFromSensor_(ReadFromSensor),
        Convert_(Filter<InputType, OutputType>::NoOpConvert) {}

  Filter(ReadFromSensorType ReadFromSensor,
         OutputType (*Convert)(InputType input))
      : ReadFromSensor_(ReadFromSensor), Convert_(Convert) {}

  // Run one iteration of the filter. Call this periodically to read the sensor
  // and run the filtering logic.
  void Run();

  // Returns the output of the filter, converted.
  OutputType GetFilteredValue() const;

  // Instructs the filter to run no more frequently than this delay. This can be
  // used to run the filter at a defined rate, which can make the filter more
  // consistent.
  //
  // One way to use this is to measure how long your main loop typically takes,
  // and set this to value several times that.
  void SetMinRunInterval(uint32_t interval_millis);

  // Controls whether to log the input and output values of this filter. Values
  // are logged to Serial in the form <input> <output>. Both input and output
  // are converted before logging. These values can be graphed using the
  // Arduino serial plotter.
  void SetLogToSerial(bool log);

  // Setters for tests
#ifndef ARDUINO
  void SetMillis(uint32_t value);
#endif

 protected:
  // This will instruct the filter to not run until at least delay milliseconds
  // have passed.
  void SetRunDelayInMillis(uint32_t delay);

  // Perform the filtering logic. Override this.
  virtual InputType DoRun() = 0;

  // Gets the raw value of the sensor.
  ReadFromSensorType ReadFromSensor_;

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
#endif

 private:
  static OutputType NoOpConvert(InputType input);

  InputType filtered_value_ = 0;
  uint32_t min_run_interval_ = 0;
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
    // Note: if min_run_interval_ is 0 (default), this will have no effect
    SetRunDelayInMillis(min_run_interval_);
  }
}

template <typename InputType, typename OutputType>
void Filter<InputType, OutputType>::SetLogToSerial(bool log) {
  log_to_serial_ = log;
}

template <typename InputType, typename OutputType>
OutputType Filter<InputType, OutputType>::GetFilteredValue() const {
  return Convert_(filtered_value_);
}

template <typename InputType, typename OutputType>
void Filter<InputType, OutputType>::SetMinRunInterval(
    uint32_t interval_millis) {
  min_run_interval_ = interval_millis;

  if (run_at_ - millis() > interval_millis) {
    run_at_ = millis() + interval_millis;
  }
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
uint32_t Filter<InputType, OutputType>::millis() {
  return fake_millis_;
}

#endif

template <typename InputType, typename OutputType>
OutputType Filter<InputType, OutputType>::NoOpConvert(InputType input) {
  return (OutputType)input;
}

namespace filter_functions {
#ifdef __AVR__
typedef bool (*BoolFunction)();
typedef uint32_t (*Uint32Function)();
#else
typedef std::function<bool()> BoolFunction;
typedef std::function<uint32_t()> Uint32Function;
#endif

template <uint32_t pin>
inline BoolFunction ForDigitalRead() {
  return []() { return (bool)digitalRead(pin); };
}

template <uint32_t pin>
inline BoolFunction ForInvertedDigitalRead() {
  return []() { return (bool)!digitalRead(pin); };
}

template <uint32_t pin>
inline Uint32Function ForAnalogRead() {
  return []() { return (uint32_t)analogRead(pin); };
}

#ifndef __AVR__
inline BoolFunction ForDigitalReadDynamic(unsigned int pin) {
  return [pin]() { return (bool)digitalRead(pin); };
}

inline BoolFunction ForInvertedDigitalReadDynamic(unsigned int pin) {
  return [pin]() { return (bool)!digitalRead(pin); };
}

inline Uint32Function ForAnalogReadDynamic(unsigned int pin) {
  return [pin]() { return (uint32_t)analogRead(pin); };
}
#endif  // __AVR__
}  // namespace filter_functions

#endif

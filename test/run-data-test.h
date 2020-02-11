#ifndef RUN_DATA_TEST
#define RUN_DATA_TEST

#include "gtest/gtest.h"

#include <cstdint>
#include <cstdio>

#include "../src/filter.h"

template <typename I, typename O>
struct InputOutput {
  I input;
  uint32_t duration_millis;
  O lower_bound;
  O upper_bound;

  InputOutput(I input, uint32_t duration_millis, O exact_value)
      : input(input),
        duration_millis(duration_millis),
        lower_bound(exact_value),
        upper_bound(exact_value) {}

  InputOutput(I input, uint32_t duration_millis, O lower_bound, O upper_bound)
      : input(input),
        duration_millis(duration_millis),
        lower_bound(lower_bound),
        upper_bound(upper_bound) {}
};

extern uint32_t analogReadValue;
extern void setAnalogRead(uint32_t value);
const auto analog_read_function = []() { return analogReadValue; };

extern float floatReadValue;
extern void setFloatRead(float value);
const auto float_read_function = []() { return floatReadValue; };

template <typename I, typename O>
void RunDataTest(Filter<I, O>* filter, std::vector<InputOutput<I, O>> data,
                 void (*setSensorValue)(I value)) {
  uint32_t millis = 0;
  uint32_t point_index = 0;
  for (auto point : data) {
    for (uint32_t i = 0; i < point.duration_millis; i++) {
      std::ostringstream debug_stream;
      debug_stream << "millis: " << millis << ", point " << point_index << ": ("
                   << point.input << ")";

      filter->SetMillis(millis);
      setSensorValue(point.input);
      filter->Run();

      EXPECT_GE(filter->GetFilteredValue(), point.lower_bound)
          << debug_stream.str();
      EXPECT_LE(filter->GetFilteredValue(), point.upper_bound)
          << debug_stream.str();

      millis++;
    }
    point_index++;
  }
}

#endif

#ifndef RUN_DATA_TEST
#define RUN_DATA_TEST

#include "gtest/gtest.h"

#include <cstdint>
#include <cstdio>

#include "../src/filter.h"

template <typename O>
struct InputOutput {
  uint32_t input;
  uint32_t duration_millis;
  O lower_bound;
  O upper_bound;

  InputOutput(uint32_t input, uint32_t duration_millis, O exact_value)
      : input(input),
        duration_millis(duration_millis),
        lower_bound(exact_value),
        upper_bound(exact_value) {}

  InputOutput(uint32_t input, uint32_t duration_millis, O lower_bound,
              O upper_bound)
      : input(input),
        duration_millis(duration_millis),
        lower_bound(lower_bound),
        upper_bound(upper_bound) {}
};

extern uint32_t analogReadValue;
extern uint32_t analogRead();

template <typename O>
void RunDataTest(Filter<uint32_t, O>* filter,
                 std::vector<InputOutput<O>> data) {
  uint32_t millis = 0;
  uint32_t point_index = 0;
  for (auto point : data) {
    for (uint32_t i = 0; i < point.duration_millis; i++) {
      std::ostringstream debug_stream;
      debug_stream << "millis: " << millis << ", point " << point_index << ": ("
                   << point.input << ")";

      filter->SetMillis(millis);
      analogReadValue = point.input;
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

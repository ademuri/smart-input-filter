#include "gtest/gtest.h"

#include "../src/debounce-filter.h"

namespace debounce_filter_test {

enum class StateChange {
  kNone,
  kRose,
  kFell,
};

struct InputOutput {
  uint8_t pin;
  bool output;
  uint32_t duration_millis;
  StateChange state_change;
};

void RunDataTest(DebounceFilter* filter, std::vector<InputOutput> &data) {
  uint32_t millis = 0;
  for (auto point : data) {
    for (uint32_t i = 0; i < point.duration_millis; i++) {
      std::ostringstream debug_stream;
      debug_stream << "millis: " << millis;

      filter->SetMillis(millis);
      filter->SetPinValue(point.pin);
      filter->Run();
      EXPECT_EQ(filter->GetFilteredValue(), point.output) << debug_stream.str();

      switch (point.state_change) {
        case StateChange::kNone:
          EXPECT_EQ(filter->Rose(), false) << debug_stream.str();
          EXPECT_EQ(filter->Fell(), false) << debug_stream.str();
          break;

        case StateChange::kRose:
          EXPECT_EQ(filter->Rose(), true) << debug_stream.str();
          EXPECT_EQ(filter->Fell(), false) << debug_stream.str();
          break;

        case StateChange::kFell:
          EXPECT_EQ(filter->Rose(), false) << debug_stream.str();
          EXPECT_EQ(filter->Fell(), true) << debug_stream.str();
          break;
      }

      millis++;
    }
  }
}

TEST(DebounceFilter, stable_filter_no_change) {
  DebounceFilter* filter = new DebounceFilter(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {1, true, 1000, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(DebounceFilter, stable_short_blip) {
  DebounceFilter* filter = new DebounceFilter(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {0, true, 5, StateChange::kNone},
    {1, true, 1000, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(DebounceFilter, stable_oscillation_low_to_high) {
  DebounceFilter* filter = new DebounceFilter(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {1, true, 4, StateChange::kNone},
    {0, true, 5, StateChange::kNone},
    {1, true, 5, StateChange::kNone},
    {0, true, 5, StateChange::kNone},
    {1, true, 1000, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(DebounceFilter, stable_oscillation_high_to_low) {
  DebounceFilter* filter = new DebounceFilter(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 10, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {1, true, 1000, StateChange::kNone},
    {0, false, 1, StateChange::kFell},
    {0, false, 4, StateChange::kNone},
    {1, false, 5, StateChange::kNone},
    {0, false, 5, StateChange::kNone},
    {1, false, 5, StateChange::kNone},
    {0, false, 1000, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(DebounceFilter, stable_oscillation_back_to_original_value) {
  DebounceFilter* filter = new DebounceFilter(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 10, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {1, true, 1000, StateChange::kNone},
    {0, false, 1, StateChange::kFell},
    {0, false, 4, StateChange::kNone},
    {1, false, 5, StateChange::kNone},
    {0, false, 5, StateChange::kNone},
    {1, false, 10, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {1, true, 1000, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(DebounceFilter, fast_change) {
  DebounceFilter* filter = new DebounceFilter(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 10, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {0, true, 10, StateChange::kNone},
    {0, false, 1, StateChange::kFell},
    {0, false, 100, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(DebounceFilter, very_fast_change) {
  DebounceFilter* filter = new DebounceFilter(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 10, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {0, true, 1, StateChange::kNone},
    {0, true, 9, StateChange::kNone},
    {0, false, 1, StateChange::kFell},
    {0, false, 100, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

}

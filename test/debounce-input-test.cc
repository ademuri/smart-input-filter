#include "gtest/gtest.h"

#include "../src/debounce-input.h"

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

void RunDataTest(DebounceInput* input, std::vector<InputOutput> data) {
  uint32_t millis = 0;
  for (auto point : data) {
    for (uint32_t i = 0; i < point.duration_millis; i++) {
      std::ostringstream debug_stream;
      debug_stream << "millis: " << millis;

      input->SetMillis(millis);
      input->SetPin(0, point.pin);
      input->Run();
      EXPECT_EQ(input->GetCurrentState(), point.output) << debug_stream.str();

      switch (point.state_change) {
        case StateChange::kNone:
          EXPECT_EQ(input->Rose(), false) << debug_stream.str();
          EXPECT_EQ(input->Fell(), false) << debug_stream.str();
          break;

        case StateChange::kRose:
          EXPECT_EQ(input->Rose(), true) << debug_stream.str();
          EXPECT_EQ(input->Fell(), false) << debug_stream.str();
          break;

        case StateChange::kFell:
          EXPECT_EQ(input->Rose(), false) << debug_stream.str();
          EXPECT_EQ(input->Fell(), true) << debug_stream.str();
          break;
      }

      millis++;
    }
  }
}

TEST(DebounceInput, stable_input_no_change) {
  DebounceInput* input = new DebounceInput(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {1, true, 1000, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(input, data);
}

TEST(DebounceInput, stable_short_blip) {
  DebounceInput* input = new DebounceInput(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {0, true, 5, StateChange::kNone},
    {1, true, 1000, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(input, data);
}

TEST(DebounceInput, stable_oscillation_low_to_high) {
  DebounceInput* input = new DebounceInput(0);
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
  RunDataTest(input, data);
}

TEST(DebounceInput, stable_oscillation_high_to_low) {
  DebounceInput* input = new DebounceInput(0);
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
  RunDataTest(input, data);
}

TEST(DebounceInput, stable_oscillation_back_to_original_value) {
  DebounceInput* input = new DebounceInput(0);
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
  RunDataTest(input, data);
}

TEST(DebounceInput, fast_change) {
  DebounceInput* input = new DebounceInput(0);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 10, StateChange::kNone},
    {1, true, 1, StateChange::kRose},
    {0, true, 10, StateChange::kNone},
    {0, false, 1, StateChange::kFell},
    {0, false, 100, StateChange::kNone},
      // clang-format on
  };
  RunDataTest(input, data);
}

TEST(DebounceInput, very_fast_change) {
  DebounceInput* input = new DebounceInput(0);
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
  RunDataTest(input, data);
}

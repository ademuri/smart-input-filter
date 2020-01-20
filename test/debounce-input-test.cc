#include "gtest/gtest.h"

#include "../src/debounce-input.h"

struct InputOutput {
  uint8_t pin;
  bool output;
  uint32_t duration_millis;
};

void RunDataTest(DebounceInput *input, std::vector<InputOutput> data) {
  uint32_t millis = 0;
  for (auto point : data) {
    for (uint32_t i = 0; i < point.duration_millis; i++) {
      input->SetMillis(millis);
      input->SetPin(0, point.pin);
      input->Run();
      EXPECT_EQ(input->GetCurrentState(), point.output) << "millis: " << millis;

      millis++;
    }
  }
}

TEST(DebounceInput, stable_input_no_change) {
  DebounceInput* input = new DebounceInput(0);
  std::vector<InputOutput> data = {
    {0, false, 1000},
    {1, true, 1000},
  };
  RunDataTest(input, data);
}

TEST(DebounceInput, stable_short_blip) {
  DebounceInput* input = new DebounceInput(0);
  std::vector<InputOutput> data = {
    {0, false, 1000},
    {1, true, 1},
    {0, true, 5},
    {1, true, 1000},
  };
  RunDataTest(input, data);
}

TEST(DebounceInput, stable_oscillation_low_to_high) {
  DebounceInput* input = new DebounceInput(0);
  std::vector<InputOutput> data = {
    {0, false, 1000},
    {1, true, 5},
    {0, true, 5},
    {1, true, 5},
    {0, true, 5},
    {1, true, 1000},
  };
  RunDataTest(input, data);
}

TEST(DebounceInput, stable_oscillation_high_to_low) {
  DebounceInput* input = new DebounceInput(0);
  std::vector<InputOutput> data = {
    {0, false, 10},
    {1, true, 1000},
    {0, false, 5},
    {1, false, 5},
    {0, false, 5},
    {1, false, 5},
    {0, false, 1000},
  };
  RunDataTest(input, data);
}

TEST(DebounceInput, stable_oscillation_back_to_original_value) {
  DebounceInput* input = new DebounceInput(0);
  std::vector<InputOutput> data = {
    {0, false, 10},
    {1, true, 1000},
    {0, false, 5},
    {1, false, 5},
    {0, false, 5},
    {1, false, 10},
    {1, true, 1000},
  };
  RunDataTest(input, data);
}

#include "gtest/gtest.h"

#include <cstdio>
#include "../src/exponential-moving-average-input.h"

namespace exponential_moving_average_input_test {

struct InputOutput {
  uint32_t input;
  uint32_t duration_millis;
  uint32_t lower_bound;
  uint32_t upper_bound;
};

void RunDataTest(ExponentialMovingAverageInput<uint32_t>* input, std::vector<InputOutput> data) {
  uint32_t millis = 0;
  for (auto point : data) {
    for (uint32_t i = 0; i < point.duration_millis; i++) {
      std::ostringstream debug_stream;
      debug_stream << "millis: " << millis;

      input->SetMillis(millis);
      input->SetPinValue(point.input);
      input->Run();

      EXPECT_LE(point.lower_bound, input->GetFilteredValue()) << debug_stream.str();
      EXPECT_GE(point.upper_bound, input->GetFilteredValue()) << debug_stream.str();

      millis++;
    }
  }
}

TEST(ExponentialMovingAverageInput, alpha_half) {
  ExponentialMovingAverageInput<uint32_t> *input = new ExponentialMovingAverageInput<uint32_t>(0, 128);
  std::vector<InputOutput> data = {
    {0, 10, 0, 0},
    {1024, 100, 4, 1024},
    {1024, 100, 1024, 1024},
    {0, 100, 0, 1020},
    {0, 100, 0, 0},
  };
  RunDataTest(input, data);
}

TEST(ExponentialMovingAverageInput, alpha_full) {
  ExponentialMovingAverageInput<uint32_t> *input = new ExponentialMovingAverageInput<uint32_t>(0, 255);
  std::vector<InputOutput> data = {
    {0, 10, 0, 0},
    {1024, 100, 1024, 1024},
    {0, 100, 0, 0},
  };
  RunDataTest(input, data);
}

TEST(ExponentialMovingAverageInput, alpha_low) {
  ExponentialMovingAverageInput<uint32_t> *input = new ExponentialMovingAverageInput<uint32_t>(0, 0);
  std::vector<InputOutput> data = {
    {0, 10, 0, 0},
    {1024, 10, 4, 50},
    {1024, 990, 4, 1024},
    {1024, 100, 1024, 1024},
    {0, 10, 974, 1020},
    {0, 1000, 0, 1020},
    {0, 100, 0, 0},
  };
  RunDataTest(input, data);
}

TEST(ExponentialMovingAverageInput, impulse) {
  ExponentialMovingAverageInput<uint32_t> *input = new ExponentialMovingAverageInput<uint32_t>(0, 127);
  std::vector<InputOutput> data = {
    {0, 10, 0, 0},
    {1024, 1, 512, 512},
    {0, 1, 256, 256},
    {0, 1, 128, 128},
    {0, 1, 64, 64},
    {0, 1, 32, 32},
    {0, 1, 16, 16},
    {0, 1, 8, 8},
    {0, 1, 4, 4},
    {0, 1, 2, 2},
    {0, 1, 1, 1},
    {0, 1, 0, 0},
    {1024, 1, 512, 512},
    {1024, 1, 768, 768},
    {1024, 1, 896, 896},
    {1024, 1, 960, 960},
    {1024, 1, 992, 992},
    {1024, 1, 1008, 1008},
    {1024, 1, 1016, 1016},
    {1024, 1, 1020, 1020},
    {1024, 1, 1022, 1022},
    {1024, 1, 1023, 1023},
    {1024, 1, 1024, 1024},
  };
  RunDataTest(input, data);
}

}

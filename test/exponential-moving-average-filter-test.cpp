#include "gtest/gtest.h"

#include <cstdio>
#include "../src/exponential-moving-average-filter.h"
#include "run-data-test.h"

namespace exponential_moving_average_filter_test {

TEST(ExponentialMovingAverageFilter, alpha_half) {
  ExponentialMovingAverageFilter<uint32_t> *filter = new ExponentialMovingAverageFilter<uint32_t>(0, 128);
  std::vector<InputOutput<uint32_t>> data = {
    {0, 10, 0},
    {1024, 100, 4, 1024},
    {1024, 100, 1024},
    {0, 100, 0, 1020},
    {0, 100, 0},
  };
  RunDataTest(filter, data);
}

TEST(ExponentialMovingAverageFilter, alpha_full) {
  ExponentialMovingAverageFilter<uint32_t> *filter = new ExponentialMovingAverageFilter<uint32_t>(0, 255);
  std::vector<InputOutput<uint32_t>> data = {
    {0, 10, 0},
    {1024, 100, 1024},
    {0, 100, 0},
  };
  RunDataTest(filter, data);
}

TEST(ExponentialMovingAverageFilter, alpha_low) {
  ExponentialMovingAverageFilter<uint32_t> *filter = new ExponentialMovingAverageFilter<uint32_t>(0, 0);
  std::vector<InputOutput<uint32_t>> data = {
    {0, 10, 0},
    {1024, 10, 4, 50},
    {1024, 990, 4, 1024},
    {1024, 100, 1024},
    {0, 10, 974, 1020},
    {0, 1000, 0, 1020},
    {0, 100, 0},
  };
  RunDataTest(filter, data);
}

TEST(ExponentialMovingAverageFilter, impulse) {
  ExponentialMovingAverageFilter<uint32_t> *filter = new ExponentialMovingAverageFilter<uint32_t>(0, 127);
  std::vector<InputOutput<uint32_t>> data = {
    {0, 10, 0},
    {1024, 1, 512},
    {0, 1, 256},
    {0, 1, 128},
    {0, 1, 64},
    {0, 1, 32},
    {0, 1, 16},
    {0, 1, 8},
    {0, 1, 4},
    {0, 1, 2},
    {0, 1, 1},
    {0, 1, 0},
    {1024, 1, 512},
    {1024, 1, 768},
    {1024, 1, 896},
    {1024, 1, 960},
    {1024, 1, 992},
    {1024, 1, 1008},
    {1024, 1, 1016},
    {1024, 1, 1020},
    {1024, 1, 1022},
    {1024, 1, 1023},
    {1024, 1, 1024},
  };
  RunDataTest(filter, data);
}

float analogInToVoltage(uint32_t analogIn) {
  return analogIn / 1023.0 * 3.3;
}

TEST(ExponentialMovingAverageFilter, convert) {
  ExponentialMovingAverageFilter<float> *filter = new ExponentialMovingAverageFilter<float>(0, 127, analogInToVoltage);
  std::vector<InputOutput<float>> data = {
    {0, 10, 0.0},
    {1023, 100, 0.0, 3.3},
    {1023, 10, 3.3},
  };
  RunDataTest(filter, data);
}

TEST(ExponentialMovingAverageFilter, filter_range) {
  // Tests that the filter supports 24-bit filters without overflow.
  // Note: 2 ^ 24 = 16777216

  ExponentialMovingAverageFilter<uint32_t> *filter = new ExponentialMovingAverageFilter<uint32_t>(0, 127);
  std::vector<InputOutput<uint32_t>> data = {
    {16777215, 500, 0, 16777215},
    {16777215, 100, 16777215},
  };
  RunDataTest(filter, data);
}

}

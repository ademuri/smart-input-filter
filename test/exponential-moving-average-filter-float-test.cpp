#include "gtest/gtest.h"

#include <cstdio>
#include "../src/exponential-moving-average-filter.h"
#include "run-data-test.h"

namespace exponential_moving_average_filter_test {

TEST(ExponentialMovingAverageFloatFilter, alpha_half) {
  ExponentialMovingAverageFilter<float, float> *filter = new ExponentialMovingAverageFilter<float, float>(floatRead, 0.5);
  std::vector<InputOutput<float, float>> data = {
    {0, 10, 0},
    {1.0, 100, 0.4, 1.0},
    {1.0, 100, 1.0},
    {0, 100, 0, 0.6},
    {0, 100, 0, 0.001},
  };
  RunDataTest(filter, data, setFloatRead);
}

TEST(ExponentialMovingAverageFloatFilter, alpha_full) {
  ExponentialMovingAverageFilter<float, float> *filter = new ExponentialMovingAverageFilter<float, float>(floatRead, 1.0);
  std::vector<InputOutput<float, float>> data = {
    {0, 10, 0},
    {10.0, 100, 10.0},
    {0, 100, 0},
  };
  RunDataTest(filter, data, setFloatRead);
}

TEST(ExponentialMovingAverageFloatFilter, alpha_low) {
  ExponentialMovingAverageFilter<float, float> *filter = new ExponentialMovingAverageFilter<float, float>(floatRead, 0.001);
  std::vector<InputOutput<float, float>> data = {
    {0, 10, 0},
    {1.0, 500, 0, 0.5},
    {1.0, 5000, 0, 1.0},
    {1.0, 100, 0.99, 1.0},
  };
  RunDataTest(filter, data, setFloatRead);
}

}

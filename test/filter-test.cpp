#include "gtest/gtest.h"

#include <cstdio>
#include "../src/exponential-moving-average-filter.h"
#include "run-data-test.h"

namespace filter_test {

TEST(Filter, SetMinRunInterval_default) {
  // Filter that returns the sensor value immediately
  ExponentialMovingAverageFilter<uint32_t> *input = new ExponentialMovingAverageFilter<uint32_t>(analogRead, 255);
  std::vector<InputOutput<uint32_t, uint32_t>> data = {
    {1, 1, 1},
    {2, 1, 2},
    {3, 1, 3},
    {4, 1, 4},
    {5, 1, 5},
  };
  RunDataTest(input, data, setAnalogRead);
}

TEST(Filter, SetMinRunInterval_10) {
  // Filter that returns the sensor value immediately
  ExponentialMovingAverageFilter<uint32_t> *input = new ExponentialMovingAverageFilter<uint32_t>(analogRead, 255);
  input->SetMinRunInterval(10);
  std::vector<InputOutput<uint32_t, uint32_t>> data = {
    {1, 1, 1},
    {2, 9, 1},

    {1, 1, 1},
    {2, 9, 1},

    {3, 10, 3},
  };
  RunDataTest(input, data, setAnalogRead);
}

TEST(Filter, SetMinRunInterval_updates) {
  ExponentialMovingAverageFilter<uint32_t> *input = new ExponentialMovingAverageFilter<uint32_t>(analogRead, 255);
  input->SetMinRunInterval(10);
  input->SetMillis(0);
  analogReadValue = 1;
  input->Run();
  EXPECT_EQ(input->GetFilteredValue(), 1);

  input->SetMinRunInterval(5);
  input->SetMillis(5);
  analogReadValue = 2;
  input->Run();
  EXPECT_EQ(input->GetFilteredValue(), 2);
}

}

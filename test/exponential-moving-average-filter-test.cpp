#include "../src/exponential-moving-average-filter.h"

#include <cstdio>
#include <functional>

#include "gtest/gtest.h"
#include "run-data-test.h"

namespace {

TEST(ExponentialMovingAverageFilter, alpha_half) {
  ExponentialMovingAverageFilter<uint32_t> filter(analog_read_function, 128);
  std::vector<InputOutput<uint32_t, uint32_t>> data = {
      {0, 10, 0},        {1024, 100, 4, 1024}, {1024, 100, 1024},
      {0, 100, 0, 1020}, {0, 100, 0},
  };
  RunDataTest(&filter, data, setAnalogRead);
}

TEST(ExponentialMovingAverageFilter, alpha_full) {
  ExponentialMovingAverageFilter<uint32_t> filter(analog_read_function, 255);
  std::vector<InputOutput<uint32_t, uint32_t>> data = {
      {0, 10, 0},
      {1024, 100, 1024},
      {0, 100, 0},
  };
  RunDataTest(&filter, data, setAnalogRead);
}

TEST(ExponentialMovingAverageFilter, alpha_low) {
  ExponentialMovingAverageFilter<uint32_t> filter(analog_read_function, 0);
  std::vector<InputOutput<uint32_t, uint32_t>> data = {
      {0, 10, 0},        {1024, 10, 4, 50},  {1024, 990, 4, 1024},
      {1024, 100, 1024}, {0, 10, 974, 1020}, {0, 1000, 0, 1020},
      {0, 100, 0},
  };
  RunDataTest(&filter, data, setAnalogRead);
}

TEST(ExponentialMovingAverageFilter, impulse) {
  ExponentialMovingAverageFilter<uint32_t> filter(analog_read_function, 127);
  std::vector<InputOutput<uint32_t, uint32_t>> data = {
      {0, 10, 0},      {1024, 1, 512},  {0, 1, 256},     {0, 1, 128},
      {0, 1, 64},      {0, 1, 32},      {0, 1, 16},      {0, 1, 8},
      {0, 1, 4},       {0, 1, 2},       {0, 1, 1},       {0, 1, 0},
      {1024, 1, 512},  {1024, 1, 768},  {1024, 1, 896},  {1024, 1, 960},
      {1024, 1, 992},  {1024, 1, 1008}, {1024, 1, 1016}, {1024, 1, 1020},
      {1024, 1, 1022}, {1024, 1, 1023}, {1024, 1, 1024},
  };
  RunDataTest(&filter, data, setAnalogRead);
}

float analogInToVoltage(uint32_t analogIn) { return analogIn / 1023.0 * 3.3; }

TEST(ExponentialMovingAverageFilter, convert) {
  ExponentialMovingAverageFilter<uint32_t, float> filter(
      analog_read_function, 127, analogInToVoltage);
  std::vector<InputOutput<uint32_t, float>> data = {
      {0, 10, 0.0},
      {1023, 100, 0.0, 3.3},
      {1023, 10, 3.3},
  };
  RunDataTest(&filter, data, setAnalogRead);
}

TEST(ExponentialMovingAverageFilter, filter_range) {
  // Tests that the filter supports 24-bit filters without overflow.
  // Note: 2 ^ 24 = 16777216

  ExponentialMovingAverageFilter<uint32_t> filter(analog_read_function, 127);
  std::vector<InputOutput<uint32_t, uint32_t>> data = {
      {16777215, 500, 0, 16777215},
      {16777215, 100, 16777215},
  };
  RunDataTest(&filter, data, setAnalogRead);
}

}  // namespace

template <typename T>
class ExponentialMovingAverageFilterTypedTest : public ::testing::Test {
 protected:
  static T sensorValue;
  static T sensor_read_function() { return sensorValue; }
  static void setSensorValue(T value) { sensorValue = value; }
};

template <typename T>
T ExponentialMovingAverageFilterTypedTest<T>::sensorValue = 0;

using FilterTypes = ::testing::Types<int32_t, uint32_t, int16_t, uint16_t>;
TYPED_TEST_SUITE(ExponentialMovingAverageFilterTypedTest, FilterTypes);

TYPED_TEST(ExponentialMovingAverageFilterTypedTest, basic_smoothing) {
  using T = TypeParam;
  ExponentialMovingAverageFilter<T> filter(this->sensor_read_function, 128);
  std::vector<InputOutput<T, T>> data = {
      {(T)0, 10, (T)0},
      {(T)100, 100, (T)0, (T)100},
      {(T)100, 100, (T)100},
  };
  RunDataTest(&filter, data, this->setSensorValue);
}

TYPED_TEST(ExponentialMovingAverageFilterTypedTest, alpha_full) {
  using T = TypeParam;
  ExponentialMovingAverageFilter<T> filter(this->sensor_read_function, 255);
  std::vector<InputOutput<T, T>> data = {
      {(T)0, 10, (T)0},
      {(T)100, 100, (T)100},
      {(T)0, 100, (T)0},
  };
  RunDataTest(&filter, data, this->setSensorValue);
}

TEST(ExponentialMovingAverageFilterSigned, negative_values) {
  ExponentialMovingAverageFilter<int32_t> filter(int_read_function, 128);
  std::vector<InputOutput<int32_t, int32_t>> data = {
      {0, 10, 0},          {-1000, 100, -1000, 0},
      {-1000, 100, -1000}, {0, 100, -1000, 0},
      {0, 100, 0},
  };
  RunDataTest(&filter, data, setIntRead);
}

TEST(ExponentialMovingAverageFilterSigned, zero_crossing) {
  ExponentialMovingAverageFilter<int32_t> filter(int_read_function, 128);
  filter.Initialize(-1000);
  std::vector<InputOutput<int32_t, int32_t>> data = {
      {-1000, 100, -1000},       {1000, 200, -1000, 1000}, {1000, 100, 1000},
      {-1000, 200, -1000, 1000}, {-1000, 100, -1000},
  };
  RunDataTest(&filter, data, setIntRead);
}

TEST(ExponentialMovingAverageFilterSigned, symmetry) {
  // Test that positive and negative transitions are symmetric.
  ExponentialMovingAverageFilter<int32_t> pos_filter(int_read_function, 128);
  ExponentialMovingAverageFilter<int32_t> neg_filter(int_read_function, 128);

  // Warm up filters
  pos_filter.Initialize(1000);
  neg_filter.Initialize(-1000);

  // Step towards zero
  setIntRead(0);
  for (int i = 0; i < 50; ++i) {
    pos_filter.Run();
    neg_filter.Run();
    EXPECT_EQ(pos_filter.GetFilteredValue(), -neg_filter.GetFilteredValue())
        << "Asymmetry at step " << i;
  }
}

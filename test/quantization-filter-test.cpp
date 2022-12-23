#include "../src/quantization-filter.h"

#include <cstdio>
#include <functional>

#include "gtest/gtest.h"
#include "run-data-test.h"

TEST(QuantizationFilter, NoOp) {
  QuantizationFilter<uint32_t> filter(analog_read_function, /*bucket_size=*/1,
                                      /*hysteresis=*/0);

  std::vector<InputOutput<uint32_t, uint32_t>> data = {
      {0, 1, 0}, {1, 1, 1}, {2, 1, 2}, {128, 1, 128}, {127, 1, 127},
  };
  RunDataTest<uint32_t, uint32_t>(&filter, data, setAnalogRead);
}

TEST(QuantizationFilter, Quantization) {
  QuantizationFilter<uint32_t> filter(analog_read_function, /*bucket_size=*/10,
                                      /*hysteresis=*/0);

  std::vector<InputOutput<uint32_t, uint32_t>> data = {
      {0, 1, 0},   {1, 1, 0},   {2, 1, 0},     {9, 1, 0},
      {10, 1, 10}, {21, 1, 20}, {109, 1, 100},
  };
  RunDataTest<uint32_t, uint32_t>(&filter, data, setAnalogRead);
}

TEST(QuantizationFilter, QuantizationAndHysteresis) {
  QuantizationFilter<uint32_t> filter(analog_read_function, /*bucket_size=*/10,
                                      /*hysteresis=*/2);

  std::vector<InputOutput<uint32_t, uint32_t>> data = {
      {0, 1, 0},   {1, 1, 0},   {2, 1, 0},   {9, 1, 0},
      {10, 1, 0},  {11, 1, 0},  {12, 1, 10}, {13, 1, 10},
      {19, 1, 10}, {20, 1, 10}, {32, 1, 30}, {18, 1, 10},
  };
  RunDataTest<uint32_t, uint32_t>(&filter, data, setAnalogRead);
}

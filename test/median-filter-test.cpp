#include "gtest/gtest.h"

#include <cstdio>
#include "../src/median-filter.h"
#include "run-data-test.h"

namespace median_filter_input_test {

TEST(MedianFilter, loading_in_order) {
  MedianFilter<uint32_t, 5> *input = new MedianFilter<uint32_t, 5>(0);
  std::vector<InputOutput<uint32_t>> data = {
    {100, 1, 100, 100},
    {200, 1, 100, 100},
    {300, 1, 200, 200},
    {400, 1, 200, 200},
    {500, 1, 300, 300},
  };
  RunDataTest(input, data);
}

TEST(MedianFilter, loading_reverse_order) {
  MedianFilter<uint32_t, 5> *input = new MedianFilter<uint32_t, 5>(0);
  std::vector<InputOutput<uint32_t>> data = {
    {500, 1, 500, 500},
    {400, 1, 400, 400},
    {300, 1, 400, 400},
    {200, 1, 300, 300},
    {100, 1, 300, 300},
  };
  RunDataTest(input, data);
}

TEST(MedianFilter, loading_random_order) {
  MedianFilter<uint32_t, 5> *input = new MedianFilter<uint32_t, 5>(0);
  std::vector<InputOutput<uint32_t>> data = {
    {500, 1, 500, 500},
    {100, 1, 100, 100},
    {400, 1, 400, 400},
    {300, 1, 300, 300},
    {200, 1, 300, 300},
  };
  RunDataTest(input, data);
}

TEST(MedianFilter, steady_state_step_function) {
  MedianFilter<uint32_t, 5> *input = new MedianFilter<uint32_t, 5>(0);
  std::vector<InputOutput<uint32_t>> data = {
    {100, 5, 100, 100},
    {200, 2, 100, 100},
    {200, 10, 200, 200},
  };
  RunDataTest(input, data);
}

TEST(MedianFilter, steady_state_impulse) {
  MedianFilter<uint32_t, 5> *input = new MedianFilter<uint32_t, 5>(0);
  std::vector<InputOutput<uint32_t>> data = {
    {100, 10, 100, 100},
    {200, 2, 100, 100},
    {100, 10, 100, 100},
    {200, 2, 100, 100},
    {200, 1, 200, 200},
    {100, 2, 200, 200},
    {100, 10, 100, 100},
  };
  RunDataTest(input, data);
}

// Not a reasonable size, but behavior should be consistent
TEST(MedianFilter, size_2) {
  MedianFilter<uint32_t, 2> *input = new MedianFilter<uint32_t, 2>(0);
  std::vector<InputOutput<uint32_t>> data = {
    {100, 1, 100, 100},
    {200, 1, 100, 100},
    {200, 2, 200, 200},
    {100, 5, 100, 100},
  };
  RunDataTest(input, data);
}

TEST(MedianFilter, size_3) {
  MedianFilter<uint32_t, 3> *input = new MedianFilter<uint32_t, 3>(0);
  std::vector<InputOutput<uint32_t>> data = {
    {100, 2, 100, 100},
    {200, 1, 100, 100},
    {200, 2, 200, 200},
    {100, 1, 200, 200},
    {100, 5, 100, 100},
  };
  RunDataTest(input, data);
}

TEST(MedianFilter, size_255) {
  MedianFilter<uint32_t, 255> *input = new MedianFilter<uint32_t, 255>(0);
  std::vector<InputOutput<uint32_t>> data = {
    {100, 255, 100, 100},
    {200, 127, 100, 100},
    {200, 1, 200, 200},
    {100, 127, 200, 200},
    {100, 1, 100, 100},
  };
  RunDataTest(input, data);
}

}
#include "../src/edge-filter.h"

#include "gtest/gtest.h"
#include "run-data-test.h"

void setAnalogReadInt32(int32_t value) { setAnalogRead(value); }

TEST(EdgeFilter, EMAFiltersInput) {
  EdgeFilter filter(analog_read_function, /*alpha=*/127);
  std::vector<InputOutput<int32_t, int32_t>> data = {
      {0, 1, 0}, {256, 1, 128}, {256, 1, 192}};
  RunDataTest<int32_t, int32_t>(&filter, data, setAnalogReadInt32);
}

TEST(EdgeFilter, DetectsEdgesNoFilter) {
  EdgeFilter filter(analog_read_function, /*alpha=*/255);

  setAnalogRead(0);
  filter.SetMillis(0);
  filter.Run();
  EXPECT_FALSE(filter.Rising(1)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(1)) << filter.Slope();
  EXPECT_TRUE(filter.Stable(1)) << filter.Slope();

  setAnalogRead(2);
  filter.SetMillis(10);
  filter.Run();
  EXPECT_TRUE(filter.Rising(1)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(1)) << filter.Slope();
  EXPECT_FALSE(filter.Stable(1)) << filter.Slope();

  setAnalogRead(0);
  filter.SetMillis(20);
  filter.Run();
  EXPECT_FALSE(filter.Rising(1)) << filter.Slope();
  EXPECT_TRUE(filter.Falling(1)) << filter.Slope();
  EXPECT_FALSE(filter.Stable(1)) << filter.Slope();

  filter.SetMillis(21);
  filter.Run();
  EXPECT_FALSE(filter.Rising(1)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(1)) << filter.Slope();
  EXPECT_TRUE(filter.Stable(1)) << filter.Slope();
}

TEST(EdgeFilter, StableBelowThreshold) {
  EdgeFilter filter(analog_read_function, /*alpha=*/255);

  setAnalogRead(0);
  filter.SetMillis(0);
  filter.Run();
  EXPECT_FALSE(filter.Rising(1)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(1)) << filter.Slope();
  EXPECT_TRUE(filter.Stable(1)) << filter.Slope();

  setAnalogRead(10);
  filter.SetMillis(10);
  filter.Run();
  EXPECT_TRUE(filter.Rising(1)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(1)) << filter.Slope();
  EXPECT_FALSE(filter.Stable(1)) << filter.Slope();

  EXPECT_TRUE(filter.Rising(9)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(9)) << filter.Slope();
  EXPECT_FALSE(filter.Stable(9)) << filter.Slope();

  EXPECT_FALSE(filter.Rising(10)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(10)) << filter.Slope();
  EXPECT_TRUE(filter.Stable(10)) << filter.Slope();

  EXPECT_FALSE(filter.Rising(256)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(256)) << filter.Slope();
  EXPECT_TRUE(filter.Stable(256)) << filter.Slope();

  setAnalogRead(0);
  filter.SetMillis(20);
  filter.Run();
  EXPECT_FALSE(filter.Rising(1)) << filter.Slope();
  EXPECT_TRUE(filter.Falling(1)) << filter.Slope();
  EXPECT_FALSE(filter.Stable(1)) << filter.Slope();

  EXPECT_FALSE(filter.Rising(9)) << filter.Slope();
  EXPECT_TRUE(filter.Falling(9)) << filter.Slope();
  EXPECT_FALSE(filter.Stable(9)) << filter.Slope();

  EXPECT_FALSE(filter.Rising(10)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(10)) << filter.Slope();
  EXPECT_TRUE(filter.Stable(10)) << filter.Slope();

  EXPECT_FALSE(filter.Rising(256)) << filter.Slope();
  EXPECT_FALSE(filter.Falling(256)) << filter.Slope();
  EXPECT_TRUE(filter.Stable(256)) << filter.Slope();
}
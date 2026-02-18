#include "../src/blink-filter.h"
#include "gtest/gtest.h"

namespace {

bool digitalReadValue = false;
bool fakeDigitalRead() { return digitalReadValue; }

/**
 * @brief Helper to simulate a signal and run the filter.
 * 
 * @param filter The filter to test.
 * @param value The digital value to set.
 * @param durationMs How many milliseconds to stay at this value.
 * @param currentTimeMs Reference to the current simulated time.
 */
void SimulateSignal(BlinkFilter& filter, bool value, uint32_t durationMs, uint32_t& currentTimeMs) {
  digitalReadValue = value;
  for (uint32_t i = 0; i < durationMs; ++i) {
    filter.SetMillis(currentTimeMs);
    filter.Run();
    currentTimeMs++;
  }
}

TEST(BlinkFilter, InitialState) {
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 4);
  EXPECT_FALSE(filter.IsBlinking());
}

TEST(BlinkFilter, PerfectBlinking) {
  // 1000ms period -> 500ms half-period. 4 lookback half-periods.
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 4);
  uint32_t currentTimeMs = 0;

  // 1st half-period (High)
  SimulateSignal(filter, true, 500, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());

  // 2nd half-period (Low)
  SimulateSignal(filter, false, 500, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());

  // 3rd half-period (High)
  SimulateSignal(filter, true, 500, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());

  // 4th half-period (Low) - Should trigger after this
  SimulateSignal(filter, false, 500, currentTimeMs);
  EXPECT_TRUE(filter.IsBlinking());

  // Continues blinking
  SimulateSignal(filter, true, 500, currentTimeMs);
  EXPECT_TRUE(filter.IsBlinking());
}

TEST(BlinkFilter, OutOfToleranceBlinking) {
  // 1000ms period -> 500ms half-period.
  // Tolerance 25/255 is ~10%, so ~50ms.
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 2);
  uint32_t currentTimeMs = 0;

  // Way too fast: 100ms half-periods
  SimulateSignal(filter, true, 100, currentTimeMs);
  SimulateSignal(filter, false, 100, currentTimeMs);
  SimulateSignal(filter, true, 100, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());

  // Way too slow: 1000ms half-periods
  SimulateSignal(filter, true, 1000, currentTimeMs);
  SimulateSignal(filter, false, 1000, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());
}

TEST(BlinkFilter, RecoveryAfterBadHalfCycle) {
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 2);
  uint32_t currentTimeMs = 0;

  // 1st half-period (Good)
  SimulateSignal(filter, true, 500, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());

  // 2nd half-period (Bad - too long)
  SimulateSignal(filter, false, 800, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());

  // 3rd half-period (Good)
  SimulateSignal(filter, true, 500, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());

  // 4th half-period (Good) - Should recover and trigger
  SimulateSignal(filter, false, 500, currentTimeMs);
  EXPECT_TRUE(filter.IsBlinking());

  // 5th half-period (Bad - too short) - Should lose blink status
  SimulateSignal(filter, true, 100, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());
}

TEST(BlinkFilter, ResetFunctionality) {
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 2);
  uint32_t currentTimeMs = 0;

  // Trigger blinking
  SimulateSignal(filter, true, 500, currentTimeMs);
  SimulateSignal(filter, false, 500, currentTimeMs);
  EXPECT_TRUE(filter.IsBlinking());

  // Reset
  filter.Reset();
  EXPECT_FALSE(filter.IsBlinking());

  // Needs 2 more good half-cycles to trigger again
  SimulateSignal(filter, true, 500, currentTimeMs);
  EXPECT_FALSE(filter.IsBlinking());
  SimulateSignal(filter, false, 500, currentTimeMs);
  EXPECT_TRUE(filter.IsBlinking());
}

}  // namespace

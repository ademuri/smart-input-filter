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
void SimulateSignal(BlinkFilter& filter, bool value, uint32_t durationMs,
                    uint32_t& currentTimeMs) {
  digitalReadValue = value;
  for (uint32_t i = 0; i < durationMs; ++i) {
    filter.SetMillis(currentTimeMs);
    filter.Run();
    currentTimeMs++;
  }
}

TEST(BlinkFilter, InitialState) {
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 4);
  EXPECT_FALSE(filter.GetFilteredValue());
}

TEST(BlinkFilter, PerfectBlinking) {
  // 1000ms period -> 500ms half-period. 4 lookback half-periods.
  // With robust logic:
  // - 1st block: Init last_input_state_
  // - 2nd block: Transition 1 (starts timer)
  // - 3rd block: Transition 2 (cons=1)
  // - 4th block: Transition 3 (cons=2)
  // - 5th block: Transition 4 (cons=3)
  // - 6th block: Transition 5 (cons=4) -> Blinking!
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 4);
  EXPECT_FALSE(filter.GetFilteredValue());
  uint32_t currentTimeMs = 0;

  SimulateSignal(filter, true, 500, currentTimeMs);   // Init
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 500, currentTimeMs);  // Trans 1
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, true, 500, currentTimeMs);   // Trans 2 (cons=1)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 500, currentTimeMs);  // Trans 3 (cons=2)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, true, 500, currentTimeMs);   // Trans 4 (cons=3)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 500, currentTimeMs);  // Trans 5 (cons=4) -> YES
  EXPECT_TRUE(filter.GetFilteredValue());
}

TEST(BlinkFilter, OutOfToleranceBlinking) {
  // 1000ms period -> 500ms half-period.
  // Tolerance 25/255 is ~10%, so ~50ms.
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 2);
  EXPECT_FALSE(filter.GetFilteredValue());
  uint32_t currentTimeMs = 0;

  // Way too fast: 100ms half-periods
  SimulateSignal(filter, true, 100, currentTimeMs);
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 100, currentTimeMs);
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, true, 100, currentTimeMs);
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 100, currentTimeMs);
  EXPECT_FALSE(filter.GetFilteredValue());

  // Way too slow: 1000ms half-periods (detected by timeout or transition)
  SimulateSignal(filter, true, 1000, currentTimeMs);
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 1000, currentTimeMs);
  EXPECT_FALSE(filter.GetFilteredValue());
}

TEST(BlinkFilter, RecoveryAfterBadHalfCycle) {
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 2);
  uint32_t currentTimeMs = 0;

  SimulateSignal(filter, true, 500, currentTimeMs);   // Init
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 500, currentTimeMs);  // Trans 1 (starts timer)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, true, 500, currentTimeMs);   // Trans 2 (cons=1)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 500, currentTimeMs);  // Trans 3 (cons=2) -> YES
  EXPECT_TRUE(filter.GetFilteredValue());

  // Timeout (Low for 800ms)
  SimulateSignal(filter, false, 800, currentTimeMs);
  EXPECT_FALSE(filter.GetFilteredValue());

  // Recover
  SimulateSignal(filter, true, 500, currentTimeMs);   // Trans 1 (starts timer)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 500, currentTimeMs);  // Trans 2 (cons=1)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, true, 500, currentTimeMs);   // Trans 3 (cons=2) -> YES
  EXPECT_TRUE(filter.GetFilteredValue());

  // Bad cycle: too short (100ms)
  SimulateSignal(filter, false, 100, currentTimeMs);
  EXPECT_TRUE(filter.GetFilteredValue());  // No transition yet
  SimulateSignal(filter, true, 500,
                 currentTimeMs);  // Transition detects it's too short
  EXPECT_FALSE(filter.GetFilteredValue());
}

TEST(BlinkFilter, ResetFunctionality) {
  BlinkFilter filter(fakeDigitalRead, 1000, 25, 2);
  uint32_t currentTimeMs = 0;

  SimulateSignal(filter, true, 500, currentTimeMs);
  SimulateSignal(filter, false, 500, currentTimeMs);
  SimulateSignal(filter, true, 500, currentTimeMs);
  SimulateSignal(filter, false, 500, currentTimeMs);
  EXPECT_TRUE(filter.GetFilteredValue());

  filter.Reset();
  EXPECT_FALSE(filter.GetFilteredValue());

  SimulateSignal(filter, true, 500,
                 currentTimeMs);  // Trans 1 (consumed by first_run_)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 500, currentTimeMs);  // Trans 2 (starts timer)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, true, 500, currentTimeMs);   // Trans 3 (cons=1)
  EXPECT_FALSE(filter.GetFilteredValue());
  SimulateSignal(filter, false, 500, currentTimeMs);  // Trans 4 (cons=2) -> YES
  EXPECT_TRUE(filter.GetFilteredValue());
}

}  // namespace

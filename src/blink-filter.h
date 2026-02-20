#ifndef SMART_INPUT_FILTER_BLINK_FILTER
#define SMART_INPUT_FILTER_BLINK_FILTER

#include "filter.h"

/**
 * @brief Detects if a digital input is toggling (blinking) at a specific
 * frequency.
 *
 * BlinkFilter monitors a digital signal and determines if it's alternating
 * between High and Low states within a specified time window (period) and
 * tolerance.
 *
 * A blinking state is confirmed when a specified number of consecutive
 * half-cycles (transitions) fall within the expected duration (period / 2) ±
 * tolerance.
 */
class BlinkFilter : public Filter<bool, bool> {
 public:
  /**
   * @brief Constructs a new BlinkFilter.
   *
   * @param ReadFromSensor A function that returns the current raw sensor state.
   * @param expectedPeriodMs The duration of a full High-Low cycle in
   * milliseconds.
   * @param toleranceFraction The allowed deviation from the expected
   * half-period. Expressed as a fraction of 255 (e.g., 25 is ~10%).
   * @param lookbackPeriods The number of consecutive half-cycles that must be
   * within tolerance to set IsBlinking() to true.
   */
  BlinkFilter(typename Filter<bool, bool>::ReadFromSensorType ReadFromSensor,
              uint32_t expectedPeriodMs, uint8_t toleranceFraction,
              uint16_t lookbackPeriods);

  // Not copyable or movable
  BlinkFilter(const BlinkFilter&) = delete;
  BlinkFilter& operator=(const BlinkFilter&) = delete;
  BlinkFilter(BlinkFilter&&) = delete;
  BlinkFilter& operator=(BlinkFilter&&) = delete;

  /**
   * @brief Returns true if the input is currently detected as blinking.
   *
   * @return true if the last `lookbackPeriods` half-cycles were within
   * tolerance.
   */
  bool IsBlinking() const;

  /**
   * @brief Resets the filter's internal state.
   *
   * Clears the consecutive half-cycle counter and sets IsBlinking() to false.
   */
  void Reset();

 protected:
  /**
   * @brief Implements the blink detection logic.
   *
   * @return true if blinking is detected, false otherwise.
   */
  bool DoRun() override;

 private:
  uint32_t expected_period_ms_;
  uint8_t tolerance_fraction_;
  uint16_t lookback_periods_;

  bool is_blinking_ = false;
  bool first_run_ = true;
  bool has_start_time_ = false;
  uint16_t consecutive_half_cycles_ = 0;
  uint32_t last_transition_time_ms_ = 0;
  bool last_input_state_ = false;
};

#endif

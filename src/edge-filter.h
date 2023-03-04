#pragma once

#include "exponential-moving-average-filter.h"
#include "filter.h"

// Looks for rising and falling edges. Applies an exponential moving average
// filter to the input, and calculates the slope of that filtered value.
class EdgeFilter : public Filter<int32_t, int32_t> {
  using Filter<int32_t, int32_t>::sensor_value_;

 public:
  EdgeFilter(
      typename Filter<int32_t, int32_t>::ReadFromSensorType read_from_sensor,
      uint8_t alpha);

  bool Rose(int32_t threshold) { return slope_ > threshold; }
  bool Fell(int32_t threshold) { return slope_ < (-threshold); }
  bool Stable(int32_t threshold) {
    return !(Rose(threshold) || Fell(threshold));
  }

  int32_t Slope() { return slope_; }

 protected:
  int32_t DoRun() override;

  void LogState() override;

 private:
  ExponentialMovingAverageFilter<int32_t> input_filter_;
  int32_t prev_value_ = 0;
  int32_t prev_value_time_ = 0;
  int32_t slope_ = 0;

  // Before calculating the slope, multiply the input by this value, to avoid
  // losing precision.
  static constexpr int32_t kValueScale = 10;
};

EdgeFilter::EdgeFilter(
    typename Filter<int32_t, int32_t>::ReadFromSensorType read_from_sensor,
    uint8_t alpha)
    : Filter<int32_t, int32_t>(read_from_sensor),
      input_filter_(read_from_sensor, alpha) {}

int32_t EdgeFilter::DoRun() {
  input_filter_.Run();

  int32_t current_time = millis();
  int32_t period = current_time - prev_value_time_;
  if (period == 0) {
    period = 1;
  }
  const int32_t current_value = input_filter_.GetFilteredValue();
  slope_ = ((current_value - prev_value_) * kValueScale) / period;

  prev_value_ = current_value;
  prev_value_time_ = current_time;
  return current_value;
}

void EdgeFilter::LogState() {
  Serial.print(Convert_(sensor_value_));
  Serial.print(" ");
  Serial.print(Convert_(GetFilteredValue()));
  Serial.print(" ");
  Serial.println(slope_);
}

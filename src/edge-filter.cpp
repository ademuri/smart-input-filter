#include "edge-filter.h"

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

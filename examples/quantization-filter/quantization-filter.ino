#include <quantization-filter.h>

constexpr uint8_t kInputPin = 0;
constexpr uint32_t kBucketSize = 4;
constexpr uint32_t kHysteresis = 2;

QuantizationFilter<uint32_t> filter(
    filter_functions::ForAnalogRead<kInputPin>(), kBucketSize, kHysteresis);

void setup() {
  pinMode(kInputPin, INPUT);
  filter.SetMinRunInterval(10);
}

void loop() {
  filter.Run();
  Serial.println(filter.GetFilteredValue() / kBucketSize);
}

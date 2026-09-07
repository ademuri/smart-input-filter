#include <quantization-filter.h>

constexpr uint8_t kInputPin = 0;
constexpr uint32_t kBucketSize = 4;
constexpr uint32_t kHysteresis = 2;

// Most users should use stack-allocated instances for better performance and
// reliability.
QuantizationFilter<uint32_t> filter(
    filter_functions::ForAnalogRead<kInputPin>(), kBucketSize, kHysteresis);

// This library also supports dynamic allocation using 'new'. While this is
// compiled here to ensure support, it is generally discouraged in embedded
// environments due to the risk of heap fragmentation.
QuantizationFilter<uint32_t>* filterPtr;

void setup() {
  pinMode(kInputPin, INPUT);
  filter.SetMinRunInterval(10);

  // Example of dynamic allocation (for compilation testing):
  filterPtr = new QuantizationFilter<uint32_t>(
      filter_functions::ForAnalogRead<kInputPin>(), kBucketSize, kHysteresis);
}

void loop() {
  filter.Run();
  Serial.println(filter.GetFilteredValue() / kBucketSize);
}

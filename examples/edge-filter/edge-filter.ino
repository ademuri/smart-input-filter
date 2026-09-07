#include <Arduino.h>
#include <edge-filter.h>

constexpr uint8_t kInputPin = 0;
constexpr int32_t kEdgeThreshold = 10;

// Most users should use stack-allocated instances for better performance and
// reliability.
EdgeFilter filter(filter_functions::ForAnalogRead<kInputPin>(), /*alpha=*/191);

// This library also supports dynamic allocation using 'new'. While this is
// compiled here to ensure support, it is generally discouraged in embedded
// environments due to the risk of heap fragmentation.
EdgeFilter* filterPtr;

void setup() {
  Serial.begin(115200);
  pinMode(kInputPin, INPUT);
  // The filter will work better if it runs at a regular interval
  filter.SetMinRunInterval(1);

  // Example of dynamic allocation (for compilation testing):
  filterPtr = new EdgeFilter(filter_functions::ForAnalogRead<kInputPin>(), 191);
}

void loop() {
  filter.Run();

  // Depending on the signal, these will print multiple times for each rising or
  // falling edge.
  if (filter.Rising(kEdgeThreshold)) {
    Serial.println("Rising edge detected");
  } else if (filter.Falling(kEdgeThreshold)) {
    Serial.println("Falling edge detected");
  }
}

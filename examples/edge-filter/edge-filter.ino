#include <Arduino.h>
#include <edge-filter.h>

constexpr uint8_t kInputPin = 0;
constexpr int32_t kEdgeThreshold = 10;

EdgeFilter filter(filter_functions::ForAnalogRead<kInputPin>(), /*alpha=*/191);

void setup() {
  Serial.begin(115200);
  pinMode(kInputPin, INPUT);
  // The filter will work better if it runs at a regular interval
  filter.SetMinRunInterval(1);
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

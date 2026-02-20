#include "blink-filter.h"
#include "debounce-filter.h"

// Define a digital pin to monitor (e.g., an optical sensor or a status LED)
const uint8_t kSensorPin = 2;
const uint8_t kLedPin = 13;

// DebounceFilter cleans up any noise or contact bounce on the input pin. This
// ensures the BlinkFilter receives a clean signal.
DebounceFilter signalDebouncer{filter_functions::ForDigitalRead<kSensorPin>()};

// BlinkFilter monitors the debounced signal and determines if it's blinking at
// the expected frequency.
// - Expected period: 1000ms (1Hz blink)
// - Tolerance: 25/255 (roughly 10% tolerance)
// - Lookback: 4 half-cycles (requires 2 full blinks to confirm)
BlinkFilter blinkDetector{[]() {
                            signalDebouncer.Run();
                            return signalDebouncer.GetFilteredValue();
                          },
                          /*expectedPeriodMs=*/1000,
                          /*toleranceFraction=*/25,
                          /*lookbaclPeriods=*/4};

void setup() {
  Serial.begin(115200);
  pinMode(kSensorPin, INPUT_PULLUP);
  pinMode(kLedPin, OUTPUT);
}

void loop() {
  // Run the blink filter logic.
  // Because the input lambda calls signalDebouncer.Run(), both filters are
  // updated in this one call.
  blinkDetector.Run();

  // Get the detection state.
  if (blinkDetector.IsBlinking()) {
    digitalWrite(kLedPin, HIGH);
  } else {
    digitalWrite(kLedPin, LOW);
  }

  // Print status to Serial
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint > 500) {
    Serial.print("Raw: ");
    Serial.print(blinkDetector.GetRawValue());
    Serial.print(" | Blinking: ");
    Serial.println(blinkDetector.IsBlinking() ? "YES" : "NO");
    lastPrint = millis();
  }
}

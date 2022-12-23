#include "run-data-test.h"

uint32_t analogReadValue = 0;
uint32_t analogRead() { return analogReadValue; }

void setAnalogRead(uint32_t value) { analogReadValue = value; }

float floatReadValue = 0.0;
float floatRead() { return floatReadValue; }

void setFloatRead(float value) { floatReadValue = value; }

#include "run-data-test.h"

uint32_t analogReadValue = 0;
uint32_t analogRead() {
  return analogReadValue;
}

#include "gtest/gtest.h"

#include "../src/debounce-input.h"

TEST(DebounceInput, stable) {
  DebounceInput* input = new DebounceInput(0);
}

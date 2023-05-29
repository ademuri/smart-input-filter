#include "button-filter.h"

#include "gtest/gtest.h"

namespace {

bool digitalReadValue = false;
int digitalRead(uint32_t pin) { return digitalReadValue; }

enum class Status {
  kNone,
  kPressed,
  kHeld,
};

struct InputOutput {
  bool pin;
  bool output;
  uint32_t duration_millis;
  Status state_change;
};

bool fakeDigitalRead() { return digitalReadValue; }

void RunDataTest(ButtonFilter* filter, std::vector<InputOutput>& data) {
  uint32_t millis = 0;
  for (auto point : data) {
    for (uint32_t i = 0; i < point.duration_millis; i++) {
      std::ostringstream debug_stream;
      debug_stream << "millis: " << millis << ", point: " << point.pin << ", "
                   << point.output << ", " << point.duration_millis;

      filter->SetMillis(millis);
      digitalReadValue = point.pin;
      filter->Run();
      EXPECT_EQ(filter->GetFilteredValue(), point.output) << debug_stream.str();

      switch (point.state_change) {
        case Status::kNone:
          EXPECT_EQ(filter->Pressed(), false) << debug_stream.str();
          EXPECT_EQ(filter->Held(), false) << debug_stream.str();
          break;

        case Status::kPressed:
          EXPECT_EQ(filter->Pressed(), true) << debug_stream.str();
          EXPECT_EQ(filter->Held(), false) << debug_stream.str();
          break;

        case Status::kHeld:
          EXPECT_EQ(filter->Pressed(), false) << debug_stream.str();
          EXPECT_EQ(filter->Held(), true) << debug_stream.str();
          break;
      }

      millis++;
    }
  }
}

TEST(ButtonFilter, PressedShort) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, Status::kNone},
    {1, true, 1, Status::kNone},
    {0, true, 10, Status::kNone},
    {0, false, 1, Status::kPressed},
    {0, false, 100, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, PressedLong) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, Status::kNone},
    {1, true, 90, Status::kNone},
    {0, false, 1, Status::kPressed},
    {0, false, 100, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, HeldShort) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, Status::kNone},
    {1, true, 100, Status::kNone},
    {0, false, 1, Status::kHeld},
    {0, false, 100, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, HeldLong) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, Status::kNone},
    {1, true, 100, Status::kNone},
    {0, false, 1, Status::kHeld},
    {0, false, 1000, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, PressedThenHeld) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, Status::kNone},
    {1, true, 10, Status::kNone},
    {0, false, 1, Status::kPressed},
    {0, false, 9, Status::kNone},
    {1, true, 100, Status::kNone},
    {0, false, 1, Status::kHeld},
    {0, false, 100, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, HeldThenPressed) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, false, 1000, Status::kNone},
    {1, true, 1000, Status::kNone},
    {0, false, 1, Status::kHeld},
    {0, false, 9, Status::kNone},
    {1, true, 10, Status::kNone},
    {0, false, 1, Status::kPressed},
    {0, false, 100, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

}  // namespace

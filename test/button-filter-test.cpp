#include "button-filter.h"

#include "gtest/gtest.h"

namespace {

bool digitalReadValue = false;
int digitalRead(uint32_t pin) { return digitalReadValue; }

enum class Status {
  kNone,
  kPressed,
  kHeld,
  kRepeated,
};

struct InputOutput {
  bool pin;
  uint32_t duration_millis;
  Status state_change;
};

bool fakeDigitalRead() { return digitalReadValue; }

void RunDataTest(ButtonFilter* filter, std::vector<InputOutput>& data,
                 uint32_t starting_millis = 0) {
  uint32_t millis = starting_millis;
  uint32_t point_index = 0;
  for (const auto& point : data) {
    for (uint32_t i = 0; i < point.duration_millis; i++) {
      std::ostringstream debug_stream;
      debug_stream << "millis: " << millis << ", point " << point_index << ": "
                   << point.pin << ", " << point.duration_millis;

      filter->SetMillis(millis);
      digitalReadValue = point.pin;
      filter->Run();

      switch (point.state_change) {
        case Status::kNone:
          EXPECT_EQ(filter->Pressed(), false) << debug_stream.str();
          EXPECT_EQ(filter->Held(), false) << debug_stream.str();
          EXPECT_EQ(filter->Repeated(), false) << debug_stream.str();
          break;

        case Status::kPressed:
          EXPECT_EQ(filter->Pressed(), true) << debug_stream.str();
          EXPECT_EQ(filter->Held(), false) << debug_stream.str();
          EXPECT_EQ(filter->Repeated(), false) << debug_stream.str();
          break;

        case Status::kHeld:
          EXPECT_EQ(filter->Pressed(), false) << debug_stream.str();
          EXPECT_EQ(filter->Held(), true) << debug_stream.str();
          EXPECT_EQ(filter->Repeated(), false) << debug_stream.str();
          break;

        case Status::kRepeated:
          EXPECT_EQ(filter->Pressed(), false) << debug_stream.str();
          EXPECT_EQ(filter->Held(), false) << debug_stream.str();
          EXPECT_EQ(filter->Repeated(), true) << debug_stream.str();
          break;
      }

      millis++;
    }
    point_index++;
  }
}

TEST(ButtonFilter, PressedShort) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0,  1000, Status::kNone},
    {1, 1, Status::kNone},
    {0, 10, Status::kNone},
    {0,  1, Status::kPressed},
    {0,  100, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, PressedLong) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0,  1000, Status::kNone},
    {1, 90, Status::kNone},
    {0,  1, Status::kPressed},
    {0,  100, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, HeldShort) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0,  1000, Status::kNone},
    {1, 100, Status::kNone},
    {1, 1, Status::kHeld},
    {0,  100, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, HeldLong) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0,  1000, Status::kNone},
    {1, 100, Status::kNone},
    {1, 1, Status::kHeld},
    {0,  1000, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, PressedThenHeld) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      // clang-format off
    {0,  1000, Status::kNone},
    {1, 10, Status::kNone},
    {0,  1, Status::kPressed},
    {0,  9, Status::kNone},
    {1, 100, Status::kNone},
    {1, 1, Status::kHeld},
    {0,  100, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, HeldThenPressed) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  std::vector<InputOutput> data = {
      {0, 1000, Status::kNone}, {1, 100, Status::kNone}, {1, 1, Status::kHeld},
      {1, 899, Status::kNone},  {0, 10, Status::kNone},  {1, 10, Status::kNone},
      {0, 1, Status::kPressed}, {0, 100, Status::kNone},
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, Repeat) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  filter->SetRepeatDelay(200);
  std::vector<InputOutput> data = {
      // clang-format off
    {0, 1000, Status::kNone},
    {1, 100, Status::kNone},
    {1, 1, Status::kHeld},
    {1, 200, Status::kNone},
    {1, 1, Status::kRepeated},
    {1, 200, Status::kNone},
    {1, 1, Status::kRepeated},
    {1, 200, Status::kNone},
    {1, 1, Status::kRepeated},
    {1, 200, Status::kNone},
    {1, 1, Status::kRepeated},
    {0, 10, Status::kNone},
    {1, 100, Status::kNone},
    {1, 1, Status::kHeld},
    {1, 200, Status::kNone},
    {1, 1, Status::kRepeated},
  };
  RunDataTest(filter, data);
}

TEST(ButtonFilter, RepeatChangeWhileHeld) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  filter->SetRepeatDelay(200);
  std::vector<InputOutput> first_data = {
    // clang-format off
    {0, 1000, Status::kNone},
    {1, 100, Status::kNone},
    {1, 1, Status::kHeld},
    {1, 200, Status::kNone},
    {1, 1, Status::kRepeated},
      // clang-format on
  };
  RunDataTest(filter, first_data);

  filter->SetRepeatDelay(300);
  std::vector<InputOutput> second_data = {
      // clang-format off
    {1, 300, Status::kNone},
    {1, 1, Status::kRepeated},
    {1, 300, Status::kNone},
    {1, 1, Status::kRepeated},
    {1, 300, Status::kNone},
    {1, 1, Status::kRepeated},
    {0, 1000, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, second_data, /*starting_millis=*/1302);
}

TEST(ButtonFilter, RepeatEnableThenDisable) {
  ButtonFilter* filter =
      new ButtonFilter(fakeDigitalRead, /*held_time_millis=*/100);
  filter->SetRepeatDelay(200);
  std::vector<InputOutput> first_data = {
      // clang-format off
    {0, 1000, Status::kNone},
    {1, 100, Status::kNone},
    {1, 1, Status::kHeld},
    {1, 200, Status::kNone},
    {1, 1, Status::kRepeated},
      // clang-format on
  };
  RunDataTest(filter, first_data);

  filter->SetRepeatDelay(0);
  std::vector<InputOutput> second_data = {
      // clang-format off
    {1, 1000, Status::kNone},
    {0, 1000, Status::kNone},
      // clang-format on
  };
  RunDataTest(filter, second_data, /*starting_millis=*/1302);
}

}  // namespace

#ifndef RUN_DATA_TEST
#define RUN_DATA_TEST

template <typename O>
struct InputOutput {
  uint32_t input;
  uint32_t duration_millis;
  O lower_bound;
  O upper_bound;
};

template <typename O>
void RunDataTest(AnalogFilter<O>* filter, std::vector<InputOutput<O>> data) {
  uint32_t millis = 0;
  uint32_t point_index = 0;
  for (auto point : data) {
    for (uint32_t i = 0; i < point.duration_millis; i++) {
      std::ostringstream debug_stream;
      debug_stream << "millis: " << millis << ", point " << point_index << ": ("
                   << point.input << ")";

      filter->SetMillis(millis);
      filter->SetPinValue(point.input);
      filter->Run();

      EXPECT_LE(point.lower_bound, filter->GetFilteredValue())
          << debug_stream.str();
      EXPECT_GE(point.upper_bound, filter->GetFilteredValue())
          << debug_stream.str();

      millis++;
    }
    point_index++;
  }
}

#endif

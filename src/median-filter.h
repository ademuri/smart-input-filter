#ifndef SMART_INPUT_FILTER_MEDIAN_FILTER
#define SMART_INPUT_FILTER_MEDIAN_FILTER

#include "filter.h"

#include <list>

namespace median_filter {
template <typename ValueType>
struct ValuePoint {
  ValueType value;
  uint8_t index;
};
}  // namespace median_filter

// A median filter. Keeps a buffer of the last <size> elements, and returns the
// median of those values.
//
// This filter has been written to be relatively fast, without sacrificing too
// much readability for speed. In the typical case (once the buffer is full), it
// iterate through the buffer at most once.
//
// For even sizes, this returns the lower median.
template <typename InputType, typename OutputType, uint8_t size>
class MedianFilter : public Filter<InputType, OutputType> {
  using Filter<InputType, OutputType>::sensor_value_;

 public:
  MedianFilter(typename Filter<InputType, OutputType>::ReadFromSensorType
                   ReadFromSensor);
  MedianFilter(
      typename Filter<InputType, OutputType>::ReadFromSensorType ReadFromSensor,
      OutputType (*Convert)(InputType input));

 protected:
  InputType DoRun() override;

 private:
  // Sorted list of recent values
  std::list<median_filter::ValuePoint<InputType>> history_;

  // The index of the next value to insert. Used to keep <size> elements in the
  // list.
  uint8_t ring_buffer_index_ = 0;

  // Set to true once history has the maximum size. Used to avoid calls to
  // history_.size, which are expensive.
  bool history_full_ = false;
};

template <typename InputType, typename OutputType, uint8_t size>
MedianFilter<InputType, OutputType, size>::MedianFilter(
    typename Filter<InputType, OutputType>::ReadFromSensorType ReadFromSensor)
    : Filter<InputType, OutputType>(ReadFromSensor) {}

template <typename InputType, typename OutputType, uint8_t size>
MedianFilter<InputType, OutputType, size>::MedianFilter(
    typename Filter<InputType, OutputType>::ReadFromSensorType ReadFromSensor,
    OutputType (*Convert)(InputType input))
    : Filter<InputType, OutputType>(ReadFromSensor, Convert) {}

template <typename InputType, typename OutputType, uint8_t size>
InputType MedianFilter<InputType, OutputType, size>::DoRun() {
  // Initial case: history has 0 or 1 elements
  if (history_.empty()) {
    history_.push_back({sensor_value_, ring_buffer_index_});
    ring_buffer_index_++;

    if (ring_buffer_index_ >= size) {
      history_full_ = true;
    }

    return sensor_value_;
  }

  // Loading case: history has enough elements to return the median, but isn't
  // full yet
  if (!history_full_) {
    OutputType median = 0;

    uint8_t new_size = history_.size() + 1;
    if (new_size == size) {
      history_full_ = true;
    }

    auto it = history_.begin();
    uint8_t i = 0;
    bool inserted = false;
    while (it != history_.end()) {
      if (!inserted && sensor_value_ < it->value) {
        it = history_.insert(it, {sensor_value_, ring_buffer_index_});
        ring_buffer_index_++;
        inserted = true;
      }

      if (new_size % 2 == 0) {
        // Even history size: pick the lower middle as the median
        if (new_size == (i + 1) * 2) {
          median = it->value;
        }
      } else {
        // Odd history size: pick the actual middle
        if (new_size == i * 2 + 1) {
          median = it->value;
        }
      }

      it++;
      i++;
    }
    if (!inserted) {
      history_.push_back({sensor_value_, ring_buffer_index_});
      ring_buffer_index_ = (ring_buffer_index_ + 1) % size;
    }

    return median;
  }

  // typical case: history is full
  uint8_t element_index_to_remove = ring_buffer_index_;
  auto it = history_.begin();
  uint8_t i = 0;
  bool inserted = false;
  bool removed = false;
  bool median_set = false;
  OutputType median = 0;
  while (it != history_.end()) {
    if (!inserted && sensor_value_ < it->value) {
      it = history_.insert(it, {sensor_value_, ring_buffer_index_});
      ring_buffer_index_ = (ring_buffer_index_ + 1) % size;
      inserted = true;
    } else {
      if (it->index == element_index_to_remove) {
        it = history_.erase(it);
        removed = true;
      }
    }

    if (size % 2 == 0) {
      // Even history size: pick the lower middle as the median
      if (size == (i + 1) * 2) {
        median = it->value;
        median_set = true;
      }
    } else {
      // Odd history size: pick the actual middle
      if (size == i * 2 + 1) {
        median = it->value;
        median_set = true;
      }
    }

    if (inserted && removed && median_set) {
      break;
    }

    it++;
    i++;
  }
  if (!inserted) {
    history_.push_back({sensor_value_, ring_buffer_index_});
    ring_buffer_index_ = (ring_buffer_index_ + 1) % size;
  }
  return median;
}

#endif

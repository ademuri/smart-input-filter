#ifndef SMART_INPUT_FILTER_MEDIAN_FILTER
#define SMART_INPUT_FILTER_MEDIAN_FILTER

#ifdef ARDUINO
// Arduino.h, automatically included by the IDE, defines min and max macros
// (which it shouldn't). Apparently FastLED depends on them, so don't undef
// them until FastLED has been included.
#undef max
#undef min
#endif

#include <list>

#include "filter.h"

namespace median_filter {
struct ValuePoint {
  uint32_t value;
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
template <typename OutputType, uint8_t size>
class MedianFilter : public Filter<uint32_t, OutputType> {
  using Filter<uint32_t, OutputType>::sensor_value_;

 public:
  MedianFilter(uint32_t (*const ReadFromSensor)());
  MedianFilter(uint32_t (*const ReadFromSensor)(),
               OutputType (*Convert)(uint32_t input));

 protected:
  uint32_t DoRun() override;

 private:
  // Sorted list of recent values
  std::list<median_filter::ValuePoint> history_;

  // The index of the next value to insert. Used to keep <size> elements in the
  // list.
  uint8_t ring_buffer_index_ = 0;

  // Set to true once history has the maximum size. Used to avoid calls to
  // history_.size, which are expensive.
  bool history_full_ = false;
};

template <typename OutputType, uint8_t size>
MedianFilter<OutputType, size>::MedianFilter(uint32_t (*const ReadFromSensor)())
    : Filter<uint32_t, OutputType>(ReadFromSensor) {}

template <typename OutputType, uint8_t size>
MedianFilter<OutputType, size>::MedianFilter(
    uint32_t (*const ReadFromSensor)(), OutputType (*Convert)(uint32_t input))
    : Filter<uint32_t, OutputType>(ReadFromSensor, Convert) {}

template <typename OutputType, uint8_t size>
uint32_t MedianFilter<OutputType, size>::DoRun() {
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

    std::list<median_filter::ValuePoint>::iterator it = history_.begin();
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
  std::list<median_filter::ValuePoint>::iterator it = history_.begin();
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

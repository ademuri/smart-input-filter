# Smart Input Filter

## Project Overview

**Smart Input Filter** is a stable, unit-tested C++ library for Arduino. It provides various input filtering algorithms to handle noisy sensor data, button debouncing, and signal smoothing.

### Key Technologies

- **Language:** C++ (C++11 standard).
- **Frameworks:** Arduino (primary target), PlatformIO (build system).
- **Testing:** Google Test (gtest) and Google Mock (gmock) for unit testing.
- **Build Tools:** CMake (for native tests), Shell scripts (for automation).

### Supported Filters

- **DebounceFilter:** Filters digital inputs (buttons/switches) to prevent bouncing.
- **ExponentialMovingAverageFilter:** Smooths analog signals using integer-based EMA.
- **MedianFilter:** Returns the median of a sliding window of samples to reject outliers.
- **QuantizationFilter:** Reduces signal resolution to filter out small fluctuations.
- **EdgeFilter:** Detects rising and falling edges.
- **ButtonFilter:** High-level filter for button interactions.

## Building and Running

### Unit Tests

The project uses Google Test for unit testing, which runs natively on your development machine.

```bash
./test.sh
```

This script creates a `build/` directory, runs CMake, compiles the tests, and executes them.

### Arduino Build Verification

To ensure the library compiles correctly for various Arduino boards, use the provided script (requires [PlatformIO](https://platformio.org/)):

```bash
# Build examples for all default boards (Teensy 3.1, Feather M0, ESP32, Uno)
./build_arduino.sh

# Build for a specific board
./build_arduino.sh esp32dev
```

### Formatting

Code follows the **Google C++ Style**. To format the codebase:

```bash
./format.sh
```

This requires `clang-format` to be installed.

## Development Conventions

### Architecture

- **Base Class:** Most filters inherit from the `Filter<InputType, OutputType>` template class in `src/filter.h`.
- **Logic:** Filtering logic is implemented by overriding the `virtual InputType DoRun()` method.
- **Execution:** Filters are typically run by calling `Run()` periodically in the main loop, then retrieving the result with `GetFilteredValue()`.

### Portability

- **AVR Support:** For boards like the Arduino Uno, the library depends on `ArduinoSTL` to provide necessary STL components.
- **Platform Mocking:** The library mocks Arduino-specific functions (`millis`, `digitalRead`, `analogRead`) when compiled for native testing (non-Arduino environment).

### Coding Style

- **Naming:**
  - Classes: `PascalCase` (e.g., `DebounceFilter`)
  - Methods/Variables: `camelCase` (e.g., `getFilteredValue`)
  - Constants: `kPascalCase` (e.g., `kDebounceTimeMillis`)
- **Headers:** Use `#ifndef` guards with the pattern `SMART_INPUT_FILTER_[FILENAME]`.
- **Standard:** Strictly C++11 to maintain compatibility with older embedded compilers.

### Testing Practices

- Every new filter or feature should include corresponding tests in the `test/` directory.
- Use `RunDataTest` (defined in `test/run-data-test.h`) for data-driven testing of filter output against expected values.

### Examples

The `examples` directory contains example usage of various filter types. This serves both as documentation for users, and a compilation test for various Arduino platforms. Every new filter should have comprehensive examples, which demonstrate all of the functionality of the filter.

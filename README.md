# Smart Input Filter

## Filters

Currently, the following filters are supported:

### DebounceInput

Filters a digital input such as a button. Prevent spurious changes in state due to button "bouncing", rapid changes when the button is pressed or released. Also provides rising and falling edges.

### ExponentialMovingAverageInput

Filters an analog input using an exponential moving average filer.

An exponential moving average filter. This uses only integer (32-bit) math.  This supports up to 24-bit inputs.

An exponential moving average filter is defined as:
   average = input_value * alpha + previous_average * (1 - alpha)
This filter takes an alpha from 0 to 255, with 255 corresponding to 1 on a typical floating-point based filter.

An alpha of 255 means that the filter returns the current value of the input. An alpha of 0 means the filtered value changes very slowly.

### MedianFilter

Keeps a buffer of recent inputs, and returns the median of those values.

This filter has been written to be relatively fast, without sacrificing too much readability for speed. In the typical case (once the buffer is full), it iterate through the buffer at most once.

For even sizes, this returns the lower median.

## Installation

If you're using PlatformIO, add `ademuri/smart-input-filter@^0.3.0` to your `lib_deps`.

If you're using the Arduino IDE, install this library by placing a copy of it in your `Arduino/libraries` folder.

If you're on a platform that doesn't have some of the C++ STL libraries (e.g. the Arduino Uno), install the [`ArduinoSTL`](https://github.com/mike-matera/ArduinoSTL) library through the Arduino library manager.

## Usage

See the `examples` directory for how to use the filters.

Generally, call the `Run` function periodically. Then, call `GetFilteredValue` to get the output of the filter.

### Debug Logging

Call `SetLogToSerial(true)` on your filter to enable debug logging. Values are logged to Serial in the form <input> <output>. These values can be graphed using the Arduino serial plotter. Both input and output are converted before logging.

### Conversion

By default, the filter will not convert the output (beyond filtering it). You can optionally pass a conversion function into the filter constructors to convert the output before returning it. For example, you could do this to convert an analogRead value (on most platforms, a 10-bit unsigned int) to a float voltage.

## Extending

The filters can use `digitalRead` and `analogRead` as inputs by passing `filter_functions::ForDigitalRead` or `filter_functions::ForAnalogRead`. If you want to use these filters with a different kind of input device, pass in a different `ReadFromSensor` function to the constructor. See the debounce example for how to do this.

## Building

To check that all code builds for Arduino, run `build_arduino.sh`. To run the unit tests, run `test.sh`.

### Requirements

- [PlatformIO core](https://docs.platformio.org/en/latest/installation.html):
    
    `sudo -H pip install platformio`
- CMake
- gcc for your platform

## Status

I'm using this library in several of my projects, and adding features as needed.

All filters have pretty good unit test coverage. Check `build_arduino.sh` for the Arduino platforms that this is compiled against.

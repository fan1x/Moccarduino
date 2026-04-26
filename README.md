![Windows MSVC Build](https://github.com/krulis-martin/Moccarduino/workflows/Windows%20MSVC%20Build/badge.svg)
![Linux GCC Build](https://github.com/krulis-martin/Moccarduino/workflows/Linux%20GCC%20Build/badge.svg)

# Moccarduino

A mocking environment for testing Arduino code. The sole objective of this project is to provide a testing platform that could be used in
[ReCodEx](https://github.com/ReCodEx) for Arduino assignments.

The emulator is operating on a well-defined API. We are not simulating any low-level aspects of the actual processors used on Arduino boards.
On the other hand, we also implement support for Funshield, which contains 3 buttons, 4 independent LEDs, and a 4-digit 7-segment LED display controlled by a shift register filled from the Arduino over a serial link.

## Limitations and differences

There are several differences from the actual Arduino that may cause problems. Please check the following issues and use the suggested workarounds so that your Arduino code works properly in Moccarduino.

### Using API functions during initialization is not possible

**Wrong example:**
```c++
class Button {
	Button() { pinMode( BTN[0], INPUT); }
};
Button button;
```
or
```c++
unsigned long t = millis();
```
**Explanation:** Emulated functions from Arduino IDE (like `pinMode` or `millis`) **MUST NOT** be called before setup (i.e., not in constructors of globally instantiated objects nor in initializers of global variables). Early emulator initialization (like in the examples) causes a signal and your program is terminated.

**Workaround:** Move any initialization that requires API functions to the `setup()` function, or to `loop()` (when first needed). You can initialize global variables with default values and then update them in `setup()`. You may create separate `init()` methods for objects that require API calls during initialization and call these methods from `setup()`.

### Differences in C++ language

- Some types have different sizes on Arduino (with respect to other compilers/platforms). Most notably, `int` is 16 bits on Arduino but 32 bits on x86. Make sure to use sufficiently large types for your variables (e.g., `unsigned long` for storing timestamps from `millis()`, instead of `int`).
```c++
int t = millis(); // BAD! ints will overflow in 65.536 seconds after startup
unsigned long t = millis(); // GOOD! (millis() returns unsigned long, which is 32 bits long)
```
- `min()` and `max()` are (templated) functions in C++ STL, but macros in Arduino IDE. This may cause problems when you mix types (e.g., `int` and `long`) as the templated function may not be able to deduce the correct type.
```c++
unsigned long ts = millis();
if (min(ts, 1000) < 1000) // COMPILE ERROR in emulator (mixing unsigned long and int)

constexpr unsigned long MAX_DELAY = 1000;
if (min(ts, MAX_DELAY) < MAX_DELAY) // GOOD! (both arguments are unsigned long)
```
- C++ uses a strict order of declarations, i.e., you need to declare functions (classes, ...) before you use them (and so it is required in the emulator). Arduino IDE is more permissive and allows you to call functions before their declaration. It is good practice to conform to the standard C++ rules and declare functions before their use.
```C++
void setup() {
	foo(); // BAD! foo is not declared yet (move it before setup)
}

void foo() {
	// ...
}
```

### Unsupported API features

- The `String` class is not supported in Moccarduino (intentionally). Use C-strings instead.
- The `Serial` interface is re-implemented in Moccarduino, but the write operations are not tested (so they can be used for debugging). Some testing scenarios feed data to the serial input, in such cases the reading methods will provide the data.
- Some features may be disabled intentionally in some testing scenarios. Particularly, the `delay()` and `delayMicroseconds()` functions are usually disabled, so the students are forced to do proper timing using API like `millis()`. Refer to the assignment description for details.

Furthermore, Moccarduino implements only functions from the documented Arduino API. Hardware functionality that requires direct access to registers (e.g., timers) is not supported. This is intentional, since one should code in a portable way (against an API) and not rely on specific hardware features.

## Code Overview

The most important part of the code is in the `shared` directory. The surrounding projects are mainly designed to demonstrate and test the code.

- `constants.hpp` holds all constants and macros as defined in Arduino IDE
- `interface.hpp`, `interface.cpp` implements the low-level API for the tested Arduino code
- `funshield.h` holds additional constants needed for the Funshield (this header is given to students for development)
- `helpers.hpp` gathers all helper classes (`BitArray`, `ShiftRegister`)
- `time_series.hpp` is a generalized implementation of a sequence of events (used for various purposes, including analytical functions useful for behavioral assertions)
- `emulator.hpp` implements the actual state of the Arduino board and provides an object-oriented interface (which is called from the C `interface`)
- `simulation.hpp` uses `emulator.hpp` and implements the controller for the simulation
- `led_display.hpp` is an implementation of a 7-segment LED display accompanied by a shift register (sequentially fed matrix control), including its demultiplexing and content decoding
- `simulation_funshield.hpp` uses `simulation.hpp` and implements higher-level simulation routines targeting specifically Funshield applications


## Credits and Disclaimer

This code is currently being developed under the [Department of Distributed and Dependable Systems](https://d3s.mff.cuni.cz/), Faculty of Mathematics and Physics, Charles University (Prague, Czech Republic). It is being tailored to our needs, and we provide no guarantees whatsoever.
 
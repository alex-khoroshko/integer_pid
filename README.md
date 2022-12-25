# integer_pid

С++ integer PID regilator. Takes 16-bit input and calculates 16-bit output.

Developed specifically for embedded systems where execution time is critical. 

Compiles both for Linux systems (for unit test and application tests/simulation) and for any embedded system. Every variable has defined size, so code should be architecture-independent (should work for 8-bit AVR, 32-bit Cortex-M series etc).

Has automatic tests for everything.

Released under [MIT License](LICENSE).

## Implemended functionality:
- P
- I
- Imax, Imin
- D
- Out_max, Out_min

## How to use

Include this repository as submodule into your project. Write code with it and build it (see below). Leave any feedback you have as issues.

### How to write code with it

1. `#include "integer_pid.h"`
1. Fill `settings_t` with proper values. 
1. Create object of type `Pid`. Pass settings into constructor.
1. Call function `iterate` as needed.

### Ordinary build

Pid regulator is defined in header using inline functions - no separate compilation is needed, it would compile as a part of user files.

**Memory consumption note**: In my experience, optimizer correctly creates only one instance of code, thus memory consumption in embedded systems is the same as for normal code placement into cpp files. If you experience excessive memory consumption - please let me know.

### Test build

Compile test: `make test`

Run test: `build/test.elf`

### Meaning of coefficients and time

Code uses coefficients with norm of 0x100. This means that if you want in float representation Kp == 1.f, then you substitute k_p = 0x100.

This rule is used for all coefficients (k_p, k_i, k_d) and time as well.

When dt == 0x100 - integrator change would be, in float representation, exactly equal to `input * k_i`, differentiation component would be exactly equal to `(input - input_previous) * k_d`.

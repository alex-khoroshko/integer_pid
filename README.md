# integer_pid

С++ integer PID regilator. Takes 16-bit input and calculates 16-bit output.

Has automatic tests for everything.

Released under [MIT License](LICENSE).

## Implemended functionality:
- P
- I
- Imax, Imin
- D
- Out_max, Out_min

## How to compile

### Ordinary build

Pid regulator is defined in header using inline functions - no separate compilation of pid is needed - it would be a part of user files.

**Memory consumption note**: In my experience, optimizer correctly creates only one instance of code, thus memory consumption for embedded systems is the same as for normal code placement into cpp files. If you experience excessive memory consumption - please let me know.

### Test build

Compile test: `make test`

Run test: `build/test.elf`

## How to use

1. `#include "integer_pid.h"`
1. Fill `settings_t` with proper values. 
1. Create object of type `Pid`. Pass settings into constructor.
1. Call function `iterate` as needed.

### Meaning of coefficients and time

Code uses coefficients with norm of 0x100. This means that if you want in float representation Kp == 1.f, then you substitute k_p = 0x100.

This rule is used for all coefficients (k_p, k_i, k_d) and time as well.

When dt == 0x100 - integrator change would be, in float representation, exactly equal to `input * k_i`, differentiation component would be exactly equal to `(input - input_previous) * k_d`.

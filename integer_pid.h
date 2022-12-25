#pragma once

#include "integer_pid_math.h"

namespace integer_pid {

class Pid {
public:
    Pid(const settings_t& cfg) : settings(cfg) {}


    int16_t iterate(int16_t input, int16_t delta_t) {
        //--- proportional part
        int32_t in_ext = input;
        int32_t out_1b = in_ext * settings.k_p;

        //--- integral part
        update_integrator(input, integrator_2b, delta_t, settings);
        out_1b += integrator_2b >> 8;

        out_1b += differentiate_1b(input, in_previous, delta_t, settings);
        in_previous = input;

        int32_t max_1b = static_cast<int32_t>(settings.out_max) << 8;
        out_1b         = out_1b > max_1b ? max_1b : out_1b;

        int32_t min_1b = static_cast<int32_t>(settings.out_min) << 8;
        out_1b         = out_1b < min_1b ? min_1b : out_1b;
        return static_cast<int16_t>(out_1b >> 8);
    }

private:
    const settings_t& settings;

    int32_t integrator_2b = 0;
    int16_t in_previous   = 0;
};

}  // namespace integer_pid

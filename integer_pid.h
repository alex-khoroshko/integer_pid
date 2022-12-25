#pragma once

#include "integer_pid_math.h"

namespace integer_pid {

class Pid {
public:
    Pid(const settings_t& cfg) : settings(cfg) {}

    /**
     * in==0xFF (one), time == 0xFF (one), ki=0xFF(one) => out = 0xFF (one)
     */
    int16_t iterate(int16_t input, int16_t delta_t) {
        //--- proportional part
        int32_t in_ext = input;
        int32_t out_1b = in_ext * settings.k_p;

        //--- integral part
        update_integrator(input, integrator_2b, delta_t, settings);
        out_1b += integrator_2b >> 8;

        out_1b += differentiate_1b(input, in_previous, delta_t, settings);
        in_previous = input;

        return out_1b >> 8;
    }

private:
    const settings_t& settings;

    int32_t integrator_2b = 0;
    int16_t in_previous   = 0;
};

}  // namespace integer_pid

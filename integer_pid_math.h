#pragma once

#include <cstdint>

namespace integer_pid {

struct settings_t {
    int16_t k_p;
    int16_t k_i;
    int16_t i_max;
    int16_t i_min;
    int16_t k_d;
    int16_t out_max;
    int16_t out_min;
};

int16_t float_to_coeff(float val) {
    const int16_t one = 0x100;
    return static_cast<int16_t>(one * val);
}
float coeff_to_float(int16_t val) {
    const int16_t one = 0x100;
    return static_cast<float>(val) / one;
}

int16_t constrain(int32_t val) {
    if (val > 0x7FFFL) {
        return 0x7FFFL;
    }
    if (val < -0x7FFFL) {
        return -0x7FFFL;
    }
    return static_cast<int16_t>(val);
}

int32_t constrain(int64_t val) {
    if (val > 0x7FFFFFFFL) {
        return 0x7FFFFFFFL;
    }
    if (val < -0x7FFFFFFFL) {
        return -0x7FFFFFFFL;
    }
    return static_cast<int32_t>(val);
}

inline void update_integrator(int16_t in, int32_t& integrator_2b,
                              uint16_t delta_t, const settings_t& settings) {
    int32_t in_ext      = in;
    int64_t i_in_1b_ext = in_ext * settings.k_i;
    int64_t i_next_val  = integrator_2b + i_in_1b_ext * delta_t;

    int16_t i_max    = settings.i_max;
    int64_t i_max_2b = static_cast<int32_t>(i_max) << 16;
    int16_t i_min    = settings.i_min;
    int64_t i_min_2b = static_cast<int32_t>(i_min) << 16;

    if (i_next_val >= i_max_2b) {
        integrator_2b = i_max;
    } else if (i_next_val <= i_min_2b) {
        integrator_2b = i_min;
    } else {
        integrator_2b = i_next_val;
    }
}


}  // namespace integer_pid

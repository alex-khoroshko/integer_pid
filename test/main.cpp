#include <cmath>
#include "integer_pid.h"
#include "simpletest.h"

using namespace integer_pid;

float integrate(float in, float ki, float integrator, float dt) {
    return integrator + in * ki * dt;
}

float differentiate(float in, float in_prev, float kd, float dt) {
    return (in - in_prev) * kd / dt;
}

char const* groups[] = {"Integrator", "Differentiator", "Global", "Finale",
                        "LeakTest"};

DEFINE_TEST_G(precision, Integrator) {
    // all coefficients for precision test must be >1, otherwise rounding error
    // would spoil the result
    float ki = 1.01f;

    settings_t cfg = {
        30,                  // k_p
        float_to_coeff(ki),  // k_i
        30000,               // i_max
        -30000,              // i_min
        100,                 // k_d
        1000,                // out_max
        -1000                // out_min
    };
    float   integrator     = -1.018f;
    int32_t integrator_int = float_to_coeff(integrator) * 0x10000;
    float   dt             = 1.1f;
    float   in             = 2.13f;

    for (int i = 0; i < 11; i++) {
        update_integrator(float_to_coeff(in), integrator_int,
                          float_to_coeff(dt), cfg);
        integrator = integrate(in, ki, integrator, dt);
    }
    float test_val = coeff_to_float(integrator_int >> 16);

    // error must fit into one bit of signed value
    bool ok = std::abs(test_val - integrator) < test_val / 127;
    TEST(ok == true);

    in = -1.234f;

    for (int i = 0; i < 11; i++) {
        update_integrator(float_to_coeff(in), integrator_int,
                          float_to_coeff(dt), cfg);
        integrator = integrate(in, ki, integrator, dt);
    }
    test_val = coeff_to_float(integrator_int >> 16);

    // error must fit into one bit of signed value
    ok = std::abs(test_val - integrator) < test_val / 127;
    TEST(ok == true);
}

DEFINE_TEST_G(imax_imin, Integrator) {
    // set imax imin to max values minus one to be sure, that they work (not
    // limiting with maximum int16_t value)
    constexpr int16_t max_val = 0x7ffe;
    settings_t        cfg     = {
        30,        // k_p
        0x100,     // k_i
        max_val,   // i_max
        -max_val,  // i_min
        100,       // k_d
        1000,      // out_max
        -1000      // out_min
    };

    float   integrator           = 0.f;
    int32_t integrator_int       = 0;
    int16_t in                   = 0x234;
    float   dt_for_10_iterations = max_val / (in * 10.1f);

    for (int i = 0; i < 10; i++) {
        update_integrator(in, integrator_int,
                          float_to_coeff(dt_for_10_iterations), cfg);
        integrator     = integrate(in, 1.f, integrator, dt_for_10_iterations);
        float test_val = integrator_int >> 16;

        // error must fit into one bit of signed value
        float margin = std::abs(test_val / 127);
        bool  ok     = std::abs(test_val - integrator) < margin;
        TEST(ok == true);
    }
    update_integrator(in, integrator_int, float_to_coeff(dt_for_10_iterations),
                      cfg);
    int16_t val = integrator_int >> 16;
    TEST(val == cfg.i_max);

    // repeat for negative value
    in             = -0x234;
    integrator_int = 0;
    integrator     = 0.f;
    for (int i = 0; i < 10; i++) {
        update_integrator(in, integrator_int,
                          float_to_coeff(dt_for_10_iterations), cfg);
        integrator     = integrate(in, 1.f, integrator, dt_for_10_iterations);
        float test_val = integrator_int >> 16;

        // error must fit into one bit of signed value
        float margin = std::abs(test_val / 127);
        bool  ok     = std::abs(test_val - integrator) < margin;
        TEST(ok == true);
    }
    update_integrator(in, integrator_int, float_to_coeff(dt_for_10_iterations),
                      cfg);
    val = integrator_int >> 16;
    TEST(val == cfg.i_min);
}

DEFINE_TEST_G(precision, Differentiator) {
    // all coefficients for precision test must be >1, otherwise rounding error
    // would spoil the result
    float kd = 1.01f;

    settings_t cfg = {
        30,                  // k_p
        100,                 // k_i
        30000,               // i_max
        -30000,              // i_min
        float_to_coeff(kd),  // k_d
        1000,                // out_max
        -1000                // out_min
    };
    float    dt     = 1.3f;
    uint16_t dt_int = float_to_coeff(dt);

    int16_t in      = 0x1555;
    int16_t in_prev = 0x3f7a;

    float reference_val = differentiate(in, in_prev, kd, dt);

    int32_t diff_int_1b = differentiate_1b(in, in_prev, dt_int, cfg);
    float   test_val    = static_cast<float>(diff_int_1b) / 0x100;

    // precision must fit into one bit of signed value
    float margin = std::abs(test_val / 127);
    bool  ok     = std::abs(test_val - reference_val) < margin;
    TEST(ok == true);
}


DEFINE_TEST_G(overflow, Differentiator) {
    settings_t cfg = {
        30,      // k_p
        100,     // k_i
        30000,   // i_max
        -30000,  // i_min
        0x7FFF,  // k_d
        1000,    // out_max
        -1000    // out_min
    };
    int16_t dt_int  = 0x1;
    int16_t in      = 0x7FFF;
    int16_t in_prev = -0x8000;

    // with dt == 0x1 - should return 0
    int32_t diff_int_1b = differentiate_1b(in, in_prev, dt_int, cfg);
    TEST(diff_int_1b == false);

    dt_int = 0x2;  // min allowed value

    // white box testing: value inside differentiation is limited to 0xFFFF and
    // then multiplied by k_d
    int32_t reference_val = 0x7FFF * cfg.k_d;

    diff_int_1b = differentiate_1b(in, in_prev, dt_int, cfg);
    bool ok     = std::abs(diff_int_1b - reference_val) < 0x2;
    TEST(ok == true);

    //--- repeat with different sign
    in            = -0x8000;
    in_prev       = 0x7FFF;
    diff_int_1b   = differentiate_1b(in, in_prev, dt_int, cfg);
    reference_val = -0x8000 * cfg.k_d;
    ok            = std::abs(diff_int_1b - reference_val) < 0x2;
    TEST(ok == true);
}


int main() {
    bool pass = true;

    for (auto group : groups)
        pass &= TestFixture::ExecuteTestGroup(group, TestFixture::Verbose);

    return pass ? 0 : 1;
}

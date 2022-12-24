#include <cmath>
#include "integer_pid_math.h"
#include "simpletest.h"

using namespace integer_pid;

float integrate(float in, float ki, float integrator, float dt) {
    return integrator + in * ki * dt;
}

char const* groups[] = {"Integrator", "Memory", "Global", "Finale", "LeakTest"};

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

    // precision must fit into one bit of signed value
    bool ok = std::abs(test_val - integrator) < test_val / 127;
    TEST(ok == true);

    in = -1.234f;

    for (int i = 0; i < 11; i++) {
        update_integrator(float_to_coeff(in), integrator_int,
                          float_to_coeff(dt), cfg);
        integrator = integrate(in, ki, integrator, dt);
    }
    test_val = coeff_to_float(integrator_int >> 16);

    // precision must fit into one bit of signed value
    ok = std::abs(test_val - integrator) < test_val / 127;
    TEST(ok == true);
}


int main() {
    bool pass = true;

    for (auto group : groups)
        pass &= TestFixture::ExecuteTestGroup(group, TestFixture::Verbose);

    return pass ? 0 : 1;
}

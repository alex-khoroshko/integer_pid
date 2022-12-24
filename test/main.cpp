#include <cmath>
#include "integer_pid_math.h"
#include "simpletest.h"

using namespace integer_pid;

float integrate(float in, float ki, float integrator, float dt) {
    return integrator + in * ki * dt;
}

char const* groups[] = {"Basic", "Memory", "Global", "Finale", "LeakTest"};

DEFINE_TEST_G(one, Basic) {
    TEST(1 + 1 == 3);
}

DEFINE_TEST_G(two, Basic) {
    float ki = 0.71f;

    settings_t cfg = {
        30,                  // k_p
        float_to_coeff(ki),  // k_i
        500,                 // i_max
        -300,                // i_min
        100,                 // k_d
        1000,                // out_max
        -1000                // out_min
    };
    float   integrator     = 0.018f;
    int32_t integrator_int = integrator * 0x100;
    float   dt             = 1.1f;
    float   in             = 2.13f;

    update_integrator(float_to_coeff(in), integrator_int, float_to_coeff(dt),
                      cfg);

    float reference_val = integrate(in, ki, integrator, dt);

    bool ok = std::abs(integrator - reference_val) < 0.01f;
    TEST(ok == true);
}


int main() {
    bool pass = true;

    for (auto group : groups)
        pass &= TestFixture::ExecuteTestGroup(group, TestFixture::Verbose);

    return pass ? 0 : 1;
}

#include "Mixer.h"
#include "config.h"

#include <stddef.h>
#include <stdint.h>

//==================================================
// PRIVATE HELPER
//==================================================

static inline float Clamp(float value, float min, float max)
{
    if (value < min)
        return min;

    if (value > max)
        return max;

    return value;
}

//==================================================
// MOTOR MIXER
//==================================================

void Mixer_Update(FlightState_t *state,
                  float rollCmd,
                  float pitchCmd,
                  float yawCmd)
{
    if (state == NULL)
        return;

    //--------------------------------------------------
    // Safety
    //--------------------------------------------------

    if (state->mode != SYSTEM_ARMED)
    {
        state->motors.m1 = ESC_MIN_US;
        state->motors.m2 = ESC_MIN_US;
        state->motors.m3 = ESC_MIN_US;
        state->motors.m4 = ESC_MIN_US;
        return;
    }

    float throttle = (float)state->setpoint.throttle;

    //--------------------------------------------------
    // X Configuration Mixer
    //
    //          Front
    //
    //     M1(CCW)    M2(CW)
    //
    //          ^
    //
    //     M4(CW)     M3(CCW)
    //
    //--------------------------------------------------

    float motor1 = throttle + rollCmd - pitchCmd - yawCmd;
    float motor2 = throttle - rollCmd - pitchCmd + yawCmd;
    float motor3 = throttle - rollCmd + pitchCmd - yawCmd;
    float motor4 = throttle + rollCmd + pitchCmd + yawCmd;

    //--------------------------------------------------
    // Clamp to ESC limits
    //--------------------------------------------------

    state->motors.m1 = (uint16_t)Clamp(motor1, ESC_MIN_US, ESC_MAX_US);
    state->motors.m2 = (uint16_t)Clamp(motor2, ESC_MIN_US, ESC_MAX_US);
    state->motors.m3 = (uint16_t)Clamp(motor3, ESC_MIN_US, ESC_MAX_US);
    state->motors.m4 = (uint16_t)Clamp(motor4, ESC_MIN_US, ESC_MAX_US);
}

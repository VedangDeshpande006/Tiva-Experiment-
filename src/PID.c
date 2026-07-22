#include "PID.h"

#include <stddef.h>

//==================================================
// Initialize PID Controller
//==================================================
void PID_Init(PID_t *pid,
              float kp,
              float ki,
              float kd,
              float windupMax,
              float outputMax)
{
    if (pid == NULL)
        return;

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->windupMax = windupMax;
    pid->outputMax = outputMax;

    PID_Reset(pid);
}


//==================================================
// Compute PID Output
//==================================================
float PID_Compute(PID_t *pid,
                  float setpoint,
                  float currentPosition,
                  float dt)
{
    if ((pid == NULL) || (dt <= 0.0f))
        return 0.0f;

    //------------------------------------------------
    // Error
    //------------------------------------------------
    float error = setpoint - currentPosition;

    //------------------------------------------------
    // Proportional
    //------------------------------------------------
    float pTerm = pid->kp * error;

    //------------------------------------------------
    // Integral
    //------------------------------------------------
    pid->integrationStored += error * dt;

    if (pid->integrationStored > pid->windupMax)
        pid->integrationStored = pid->windupMax;

    if (pid->integrationStored < -pid->windupMax)
        pid->integrationStored = -pid->windupMax;

    float iTerm = pid->ki * pid->integrationStored;

    //------------------------------------------------
    // Derivative
    //------------------------------------------------
    float derivative = (error - pid->prevError) / dt;
    float dTerm = pid->kd * derivative;

    pid->prevError = error;

    //------------------------------------------------
    // Total Output
    //------------------------------------------------
    float output = pTerm + iTerm + dTerm;

    //------------------------------------------------
    // Output Saturation
    //------------------------------------------------
    if (output > pid->outputMax)
        output = pid->outputMax;

    if (output < -pid->outputMax)
        output = -pid->outputMax;

    return output;
}


//==================================================
// Reset PID
//==================================================
void PID_Reset(PID_t *pid)
{
    if (pid == NULL)
        return;

    pid->prevError = 0.0f;
    pid->integrationStored = 0.0f;
}

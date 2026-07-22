#ifndef PID_H
#define PID_H

#include <stdbool.h>

//==================================================
// PID CONTROLLER
//==================================================
typedef struct
{
    // -----------------------------
    // PID Gains
    // -----------------------------
    float kp;
    float ki;
    float kd;

    // -----------------------------
    // State Variables
    // -----------------------------
    float prevError;
    float integral;

    // -----------------------------
    // Integral Limits (Anti-Windup)
    // -----------------------------
    float integralMin;
    float integralMax;

    // -----------------------------
    // Output Limits
    // -----------------------------
    float outputMin;
    float outputMax;

} PID_t;


//==================================================
// API
//==================================================

/**
 * @brief Initialize PID controller.
 *
 * @param pid Pointer to PID object
 * @param kp Proportional gain
 * @param ki Integral gain
 * @param kd Derivative gain
 * @param integralMin Minimum integral accumulator
 * @param integralMax Maximum integral accumulator
 * @param outputMin Minimum controller output
 * @param outputMax Maximum controller output
 */
void PID_Init(PID_t *pid,
              float kp,
              float ki,
              float kd,
              float integralMin,
              float integralMax,
              float outputMin,
              float outputMax);

/**
 * @brief Calculate PID output.
 *
 * @param pid Pointer to PID object
 * @param setpoint Desired value
 * @param measurement Current measured value
 * @param dt Loop time (seconds)
 *
 * @return PID controller output
 */
float PID_Compute(PID_t *pid,
                  float setpoint,
                  float measurement,
                  float dt);

/**
 * @brief Reset integral and derivative history.
 *
 * @param pid Pointer to PID object
 */
void PID_Reset(PID_t *pid);

#endif /* PID_H */

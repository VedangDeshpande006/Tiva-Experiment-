#ifndef PID_H
#define PID_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//==================================================
// PID Controller Structure
//==================================================
typedef struct
{
    //------------------------------------------------
    // PID Gains
    //------------------------------------------------
    float kp;
    float ki;
    float kd;

    //------------------------------------------------
    // Controller State
    //------------------------------------------------
    float prevError;
    float integrationStored;

    //------------------------------------------------
    // Output Limits
    //------------------------------------------------
    float windupMax;
    float outputMax;

} PID_t;


//==================================================
// Public API
//==================================================

/**
 * @brief Initialize a PID controller.
 *
 * @param pid Pointer to PID object.
 * @param kp Proportional gain.
 * @param ki Integral gain.
 * @param kd Derivative gain.
 * @param windupMax Maximum integral term.
 * @param outputMax Maximum controller output.
 */
void PID_Init(PID_t *pid,
              float kp,
              float ki,
              float kd,
              float windupMax,
              float outputMax);


/**
 * @brief Compute PID output.
 *
 * @param pid Pointer to PID object.
 * @param setpoint Desired value.
 * @param currentPosition Measured value.
 * @param dt Loop time in seconds.
 *
 * @return PID output.
 */
float PID_Compute(PID_t *pid,
                  float setpoint,
                  float currentPosition,
                  float dt);


/**
 * @brief Reset PID internal state.
 *
 * @param pid Pointer to PID object.
 */
void PID_Reset(PID_t *pid);

#ifdef __cplusplus
}
#endif

#endif /* PID_H */

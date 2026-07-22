#include "Kalman.h"
#include "config.h"

#include <stddef.h>

//==================================================
// Initialize Kalman Filter
//==================================================
void Kalman_Init(Kalman_t *filter)
{
    if (filter == NULL)
        return;

    //------------------------------------------------
    // Load tuning parameters
    //------------------------------------------------
    filter->Q_angle   = KALMAN_Q_ANGLE;
    filter->Q_bias    = KALMAN_Q_BIAS;
    filter->R_measure = KALMAN_R_MEASURE;

    //------------------------------------------------
    // Initial state
    //------------------------------------------------
    filter->angle = 0.0f;
    filter->bias  = 0.0f;
    filter->rate  = 0.0f;

    //------------------------------------------------
    // Covariance matrix
    //------------------------------------------------
    filter->P[0][0] = 0.0f;
    filter->P[0][1] = 0.0f;
    filter->P[1][0] = 0.0f;
    filter->P[1][1] = 0.0f;
}


//==================================================
// Kalman Filter Update
//==================================================
float Kalman_GetAngle(Kalman_t *filter,
                      float newAngle,
                      float newRate,
                      float dt)
{
    if (filter == NULL)
        return 0.0f;

    //------------------------------------------------
    // Prediction Step
    //------------------------------------------------

    filter->rate = newRate - filter->bias;

    filter->angle += dt * filter->rate;

    filter->P[0][0] += dt * (dt * filter->P[1][1]
                           - filter->P[0][1]
                           - filter->P[1][0]
                           + filter->Q_angle);

    filter->P[0][1] -= dt * filter->P[1][1];
    filter->P[1][0] -= dt * filter->P[1][1];
    filter->P[1][1] += filter->Q_bias * dt;

    //------------------------------------------------
    // Measurement Update
    //------------------------------------------------

    float S = filter->P[0][0] + filter->R_measure;

    float K0 = filter->P[0][0] / S;
    float K1 = filter->P[1][0] / S;

    float innovation = newAngle - filter->angle;

    filter->angle += K0 * innovation;
    filter->bias  += K1 * innovation;

    //------------------------------------------------
    // Update Covariance Matrix
    //------------------------------------------------

    float P00 = filter->P[0][0];
    float P01 = filter->P[0][1];
    float P10 = filter->P[1][0];
    float P11 = filter->P[1][1];

    filter->P[0][0] = P00 - K0 * P00;
    filter->P[0][1] = P01 - K0 * P01;
    filter->P[1][0] = P10 - K1 * P00;
    filter->P[1][1] = P11 - K1 * P01;

    return filter->angle;
}

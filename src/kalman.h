#ifndef KALMAN_H
#define KALMAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//==================================================
// KALMAN FILTER STATE
//==================================================
typedef struct
{
    // Process noise variance for accelerometer
    float Q_angle;

    // Process noise variance for gyro bias
    float Q_bias;

    // Measurement noise variance
    float R_measure;

    // Estimated angle
    float angle;

    // Estimated gyro bias
    float bias;

    // Unbiased gyro rate
    float rate;

    // Error covariance matrix
    float P[2][2];

} Kalman_t;


//==================================================
// PUBLIC API
//==================================================

/**
 * @brief Initializes the Kalman filter structure.
 *
 * @param filter Pointer to Kalman filter instance.
 */
void Kalman_Init(Kalman_t *filter);


/**
 * @brief Updates the Kalman filter and returns the filtered angle.
 *
 * @param filter Pointer to Kalman filter instance.
 * @param newAngle Angle measured from accelerometer (degrees)
 * @param newRate Gyroscope angular rate (degrees/sec)
 * @param dt Time step (seconds)
 *
 * @return Filtered angle (degrees)
 */
float Kalman_GetAngle(Kalman_t *filter,
                      float newAngle,
                      float newRate,
                      float dt);

#ifdef __cplusplus
}
#endif

#endif /* KALMAN_H */

should i commit this code 

\#ifndef KALMAN\_H
\#define KALMAN\_H

\#include \<stdint.h>
\#include \<stdbool.h>

\#ifdef \_\_cplusplus
extern "C" {
\#endif

/\*=============================================================================

- KALMAN FILTER STATE
  *===========================================================================*/

typedef struct
{
/\* Process noise variance for accelerometer angle \*/
float Q\_angle;

```
/* Process noise variance for gyro bias */
float Q_bias;

/* Measurement noise variance */
float R_measure;

/* Estimated angle (degrees) */
float angle;

/* Estimated gyro bias (degrees/sec) */
float bias;

/* Unbiased gyro rate (degrees/sec) */
float rate;

/* Error covariance matrix */
float P[2][2];
```

} Kalman\_t;

/\*=============================================================================

- PUBLIC API
  *===========================================================================*/

/\*\*

- @brief Initialize a Kalman filter instance.
-
- Resets the estimated angle, gyro bias, rate and covariance matrix.
-
- @param filter Pointer to Kalman filter instance.
  \*/
  void Kalman\_Init(Kalman\_t \*filter);

/\*\*

- @brief Update the Kalman filter.
-
- Combines:
-
  - Accelerometer-derived angle
-
  - Gyroscope angular rate
-
- to produce a filtered attitude angle.
-
- @param filter  Pointer to Kalman filter instance.
- @param newAngle Accelerometer-derived angle in degrees.
- @param newRate  Gyroscope rate in degrees/second.
- @param dt       Control-loop timestep in seconds.
-
- @return Filtered angle in degrees.
  \*/
  float Kalman\_GetAngle(
  Kalman\_t \*filter,
  float newAngle,
  float newRate,
  float dt
  );

\#ifdef \_\_cplusplus
}
\#endif

\#endif /\* KALMAN\_H \*/

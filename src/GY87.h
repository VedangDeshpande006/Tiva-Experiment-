#ifndef GY87_H
#define GY87_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//==================================================
// MPU6050 IMU Raw Data
//==================================================

typedef struct
{
    int16_t ax;
    int16_t ay;
    int16_t az;

    int16_t gx;
    int16_t gy;
    int16_t gz;

    float temperature;

} GY87_IMU_t;


//==================================================
// HMC5883L Magnetometer
//==================================================

typedef struct
{
    int16_t mx;
    int16_t my;
    int16_t mz;

    float heading;

} GY87_Magnetometer_t;


//==================================================
// BMP180 Barometer (Reserved)
//==================================================
//
// Phase-1:
//
// The BMP180 hardware exists on the GY-87 module
// but altitude estimation is intentionally deferred
// until Phase-2.
//
// Keeping this structure now avoids API changes
// later.
//

typedef struct
{
    bool initialized;

    int32_t pressure;

    float temperature;

    float altitude;

} GY87_Barometer_t;


//==================================================
// Complete Sensor State
//==================================================

typedef struct
{
    bool imuHealthy;

    bool magnetometerHealthy;

    bool barometerHealthy;

    GY87_IMU_t imu;

    GY87_Magnetometer_t magnetometer;

    GY87_Barometer_t barometer;

} GY87_State_t;


//==================================================
// Public API
//==================================================

/**
 * @brief Initialize the complete GY-87 module.
 *
 * Initializes:
 *
 *  • MPU6050
 *  • HMC5883L
 *
 * BMP180 initialization is reserved for Phase-2.
 *
 * @return true if all mandatory sensors initialize.
 */
bool GY87_Init(void);


/**
 * @brief Read all available sensors.
 *
 * Updates:
 *
 *  • Accelerometer
 *  • Gyroscope
 *  • Temperature
 *  • Magnetometer
 *
 * Barometer support will be added later.
 */
void GY87_UpdateState(GY87_State_t *state);


/**
 * @brief Returns true if the IMU is responding.
 */
bool GY87_IMU_Healthy(void);


/**
 * @brief Returns true if the magnetometer is responding.
 */
bool GY87_Magnetometer_Healthy(void);


/**
 * @brief Returns true if the BMP180 is initialized.
 *
 * Phase-1:
 *
 * Always returns false because the BMP180 driver
 * is not yet implemented.
 */
bool GY87_Barometer_Healthy(void);


#ifdef __cplusplus
}
#endif

#endif /* GY87_H */

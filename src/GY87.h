#ifndef GY87_H
#define GY87_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//==================================================
// MPU6050 IMU Data
//==================================================

typedef struct
{
    /* Raw Accelerometer */
    int16_t ax;
    int16_t ay;
    int16_t az;

    /* Raw Gyroscope */
    int16_t gx;
    int16_t gy;
    int16_t gz;

    /* Temperature (°C) */
    float temperature;

    /* Scaled Accelerometer (g) */
    float accelX_g;
    float accelY_g;
    float accelZ_g;

    /* Scaled Gyroscope (deg/s) */
    float gyroX_dps;
    float gyroY_dps;
    float gyroZ_dps;

} GY87_IMU_t;


//==================================================
// HMC5883L Magnetometer
//==================================================

typedef struct
{
    /* Raw Magnetometer */
    int16_t mx;
    int16_t my;
    int16_t mz;

    /* Scaled Magnetic Field (Gauss) */
    float fieldX;
    float fieldY;
    float fieldZ;

    /* Heading (Radians) */
    float heading;

} GY87_Magnetometer_t;


//==================================================
// BMP180 Barometer (Phase-2 Reserved)
//==================================================

typedef struct
{
    bool initialized;

    int32_t rawPressure;
    int32_t rawTemperature;

    float pressure_hPa;
    float temperature;
    float altitude_m;

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
 * @brief Initialize the GY-87 sensor module.
 *
 * Initializes:
 *   - MPU6050
 *   - HMC5883L
 *
 * BMP180 support is reserved for Phase-2.
 *
 * @return true if mandatory sensors initialize successfully.
 */
bool GY87_Init(void);


/**
 * @brief Read MPU6050 IMU.
 *
 * @param imu Pointer to destination structure.
 *
 * @return true on success.
 */
bool GY87_ReadIMU(GY87_IMU_t *imu);


/**
 * @brief Read HMC5883L magnetometer.
 *
 * @param mag Pointer to destination structure.
 *
 * @return true on success.
 */
bool GY87_ReadMagnetometer(GY87_Magnetometer_t *mag);


/**
 * @brief Read BMP180 barometer.
 *
 * Phase-1 placeholder.
 *
 * @param baro Pointer to destination structure.
 *
 * @return true if BMP180 was detected.
 */
bool GY87_ReadBarometer(GY87_Barometer_t *baro);


/**
 * @brief Read every available sensor and update the state structure.
 *
 * @param state Pointer to complete sensor state.
 */
void GY87_UpdateState(GY87_State_t *state);


/**
 * @brief Returns true if the MPU6050 is healthy.
 */
bool GY87_IMU_Healthy(void);


/**
 * @brief Returns true if the HMC5883L is healthy.
 */
bool GY87_Magnetometer_Healthy(void);


/**
 * @brief Returns true if the BMP180 was detected.
 *
 * During Phase-1 the driver is only detected,
 * not fully implemented.
 */
bool GY87_Barometer_Healthy(void);


#ifdef __cplusplus
}
#endif

#endif /* GY87_H */

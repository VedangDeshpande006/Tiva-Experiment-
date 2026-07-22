#ifndef GY87_H
#define GY87_H

#include <stdint.h>
#include <stdbool.h>

//==================================================
// MPU6050 DATA
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

} MPU6050_Data_t;


//==================================================
// HMC5883L DATA
//==================================================
typedef struct
{
    int16_t mx;
    int16_t my;
    int16_t mz;

    // Magnetic heading (0 to 2π radians)
    float heading;

} HMC5883L_Data_t;


//==================================================
// COMPLETE GY-87 SENSOR STATE
//==================================================
typedef struct
{
    MPU6050_Data_t imu;
    HMC5883L_Data_t magnetometer;

    /* Future Expansion
    BMP180_Data_t barometer;
    */

} GY87_State_t;


//==================================================
// DRIVER FUNCTIONS
//==================================================

/**
 * @brief Initialize the MPU6050 and HMC5883L sensors.
 *
 * @return true  Initialization successful.
 * @return false Sensor not detected or I2C communication failed.
 */
bool GY87_Init(void);

/**
 * @brief Read all GY-87 sensors and update the state structure.
 *
 * @param state Pointer to GY87_State_t.
 */
void GY87_UpdateState(GY87_State_t *state);

#endif /* GY87_H */

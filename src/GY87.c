/******************************************************************************
 * File    : GY87.c
 * Author  : Project Autonomous Quadcopter
 * Target  : TM4C123GH6PM (Tiva C LaunchPad)
 *
 * Driver for the GY-87 IMU Module
 *
 * Sensors:
 *  - MPU6050  (Accelerometer + Gyroscope)
 *  - HMC5883L (Magnetometer)
 *  - BMP180   (Barometer - Phase 2)
 ******************************************************************************/

#include "GY87.h"
#include "config.h"
#include "I2C_Utils.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/******************************************************************************
 * MPU6050 Registers
 ******************************************************************************/

#define MPU6050_SMPLRT_DIV        0x19
#define MPU6050_CONFIG            0x1A
#define MPU6050_GYRO_CONFIG       0x1B
#define MPU6050_ACCEL_CONFIG      0x1C
#define MPU6050_INT_PIN_CFG       0x37
#define MPU6050_ACCEL_XOUT_H      0x3B
#define MPU6050_USER_CTRL         0x6A
#define MPU6050_PWR_MGMT_1        0x6B
#define MPU6050_WHO_AM_I          0x75

#define MPU6050_DEVICE_ID         0x68

/******************************************************************************
 * HMC5883L Registers
 ******************************************************************************/

#define HMC5883L_CONFIG_A         0x00
#define HMC5883L_CONFIG_B         0x01
#define HMC5883L_MODE             0x02
#define HMC5883L_DATA_START       0x03
#define HMC5883L_ID_A             0x0A

#define HMC5883L_DEVICE_ID        0x48

/* HMC5883L Configuration Values */
#define HMC5883L_CONFIG_A_VALUE   0x70
#define HMC5883L_CONFIG_B_VALUE   0x20
#define HMC5883L_CONTINUOUS_MODE  0x00

/******************************************************************************
 * BMP180 Registers
 ******************************************************************************/

#define BMP180_CHIP_ID            0xD0
#define BMP180_CHIP_VALUE         0x55

/******************************************************************************
 * Conversion Constants
 ******************************************************************************/

#define MPU6050_TEMP_SENSITIVITY  340.0f
#define MPU6050_TEMP_OFFSET       36.53f

/******************************************************************************
 * Driver State
 ******************************************************************************/

static bool g_imuHealthy = false;
static bool g_magHealthy = false;
static bool g_baroHealthy = false;

/******************************************************************************
 * Local Helpers
 ******************************************************************************/

static inline int16_t GY87_CombineBytes(uint8_t msb, uint8_t lsb)
{
    return (int16_t)(((uint16_t)msb << 8) | lsb);
}

static float GY87_ComputeHeading(float mx, float my)
{
    float heading = atan2f(my, mx);

    if (heading < 0.0f)
    {
        heading += (2.0f * M_PI);
    }

    return heading;
}

/******************************************************************************
 * Enable MPU6050 I2C Bypass
 ******************************************************************************/

static bool MPU6050_EnableBypass(void)
{
    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_USER_CTRL,
                       0x00))
    {
        return false;
    }

    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_INT_PIN_CFG,
                       0x02))
    {
        return false;
    }

    return true;
}

/******************************************************************************
 * MPU6050 Initialization
 ******************************************************************************/

static bool MPU6050_Init(void)
{
    uint8_t id = 0;

    if (!I2C_ReadBurst(MPU6050_I2C_ADDRESS,
                       MPU6050_WHO_AM_I,
                       &id,
                       1))
    {
        return false;
    }

    if (id != MPU6050_DEVICE_ID)
    {
        return false;
    }

    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_PWR_MGMT_1,
                       0x00))
    {
        return false;
    }

    SysCtlDelay((SYSTEM_CLOCK_HZ / 3000U) * SENSOR_STARTUP_DELAY_MS);

    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_SMPLRT_DIV,
                       MPU6050_SMPLRT_DIV_VALUE))
    {
        return false;
    }

    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_CONFIG,
                       MPU6050_DLPF_CFG))
    {
        return false;
    }

    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_GYRO_CONFIG,
                       MPU6050_GYRO_FS_2000))
    {
        return false;
    }

    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_ACCEL_CONFIG,
                       MPU6050_ACCEL_FS_8))
    {
        return false;
    }

    if (!MPU6050_EnableBypass())
    {
        return false;
    }

    return true;
}

/******************************************************************************
 * HMC5883L Initialization
 ******************************************************************************/

static bool HMC5883L_Init(void)
{
    uint8_t id = 0;

    if (!I2C_ReadBurst(HMC5883L_I2C_ADDRESS,
                       HMC5883L_ID_A,
                       &id,
                       1))
    {
        return false;
    }

    if (id != HMC5883L_DEVICE_ID)
    {
        return false;
    }

    if (!I2C_WriteByte(HMC5883L_I2C_ADDRESS,
                       HMC5883L_CONFIG_A,
                       HMC5883L_CONFIG_A_VALUE))
    {
        return false;
    }

    if (!I2C_WriteByte(HMC5883L_I2C_ADDRESS,
                       HMC5883L_CONFIG_B,
                       HMC5883L_CONFIG_B_VALUE))
    {
        return false;
    }

    if (!I2C_WriteByte(HMC5883L_I2C_ADDRESS,
                       HMC5883L_MODE,
                       HMC5883L_CONTINUOUS_MODE))
    {
        return false;
    }

    return true;
}

/******************************************************************************
 * BMP180 Detection
 ******************************************************************************/

static bool BMP180_Init(void)
{
    uint8_t id = 0;

    if (!I2C_ReadBurst(BMP180_I2C_ADDRESS,
                       BMP180_CHIP_ID,
                       &id,
                       1))
    {
        return false;
    }

    return (id == BMP180_CHIP_VALUE);
}

/******************************************************************************
 * Public Initialization
 ******************************************************************************/

bool GY87_Init(void)
{
    g_imuHealthy = MPU6050_Init();

    g_magHealthy = false;
    g_baroHealthy = false;

    if (g_imuHealthy)
    {
        g_magHealthy = HMC5883L_Init();
    }

    g_baroHealthy = BMP180_Init();

    /* Flight only strictly requires the MPU6050. Graceful degradation if mag fails. */
    return g_imuHealthy;
}

/******************************************************************************
 * Read MPU6050 IMU
 ******************************************************************************/

bool GY87_ReadIMU(GY87_IMU_t *imu)
{
    if (imu == NULL)
    {
        return false;
    }

    uint8_t buffer[14];

    if (!I2C_ReadBurst(MPU6050_I2C_ADDRESS,
                       MPU6050_ACCEL_XOUT_H,
                       buffer,
                       sizeof(buffer)))
    {
        return false;
    }

    imu->ax = GY87_CombineBytes(buffer[0], buffer[1]);
    imu->ay = GY87_CombineBytes(buffer[2], buffer[3]);
    imu->az = GY87_CombineBytes(buffer[4], buffer[5]);

    int16_t rawTemp = GY87_CombineBytes(buffer[6], buffer[7]);

    imu->temperature =
        ((float)rawTemp / MPU6050_TEMP_SENSITIVITY) +
        MPU6050_TEMP_OFFSET;

    imu->gx = GY87_CombineBytes(buffer[8], buffer[9]);
    imu->gy = GY87_CombineBytes(buffer[10], buffer[11]);
    imu->gz = GY87_CombineBytes(buffer[12], buffer[13]);

    imu->accelX_g = ((float)imu->ax) * ACCEL_SCALE;
    imu->accelY_g = ((float)imu->ay) * ACCEL_SCALE;
    imu->accelZ_g = ((float)imu->az) * ACCEL_SCALE;

    imu->gyroX_dps = ((float)imu->gx) * GYRO_SCALE;
    imu->gyroY_dps = ((float)imu->gy) * GYRO_SCALE;
    imu->gyroZ_dps = ((float)imu->gz) * GYRO_SCALE;

    return true;
}

/******************************************************************************
 * Read HMC5883L Magnetometer
 ******************************************************************************/

bool GY87_ReadMagnetometer(GY87_Magnetometer_t *mag)
{
    if (mag == NULL)
    {
        return false;
    }

    uint8_t buffer[6];

    if (!I2C_ReadBurst(HMC5883L_I2C_ADDRESS,
                       HMC5883L_DATA_START,
                       buffer,
                       sizeof(buffer)))
    {
        return false;
    }

    /*
     * HMC5883L Output Order:
     * X, Z, Y
     */

    mag->mx = GY87_CombineBytes(buffer[0], buffer[1]);
    mag->mz = GY87_CombineBytes(buffer[2], buffer[3]);
    mag->my = GY87_CombineBytes(buffer[4], buffer[5]);

    /*
     * Default sensitivity:
     * 1090 LSB/Gauss
     */

    mag->fieldX = (float)mag->mx / 1090.0f;
    mag->fieldY = (float)mag->my / 1090.0f;
    mag->fieldZ = (float)mag->mz / 1090.0f;

    mag->heading = GY87_ComputeHeading(
        mag->fieldX,
        mag->fieldY);

    return true;
}

/******************************************************************************
 * Read BMP180 (Phase-1 Placeholder)
 ******************************************************************************/

bool GY87_ReadBarometer(GY87_Barometer_t *baro)
{
    if (baro == NULL)
    {
        return false;
    }

    /*
     * Phase-2:
     *  - Read calibration EEPROM
     *  - Read uncompensated temperature
     *  - Read uncompensated pressure
     *  - Compute compensated pressure
     *  - Compute altitude
     */

    baro->initialized = g_baroHealthy;

    baro->rawPressure = 0;
    baro->rawTemperature = 0;

    baro->pressure_hPa = 0.0f;
    baro->temperature = 0.0f;
    baro->altitude_m = 0.0f;

    return g_baroHealthy;
}

/******************************************************************************
 * Update Complete Sensor State
 ******************************************************************************/

void GY87_UpdateState(GY87_State_t *state)
{
    if (state == NULL)
    {
        return;
    }

    state->imuHealthy = GY87_ReadIMU(&state->imu);

    state->magnetometerHealthy =
        GY87_ReadMagnetometer(&state->magnetometer);

    state->barometerHealthy =
        GY87_ReadBarometer(&state->barometer);

    /*
     * Update global health flags so that
     * health getter functions always reflect
     * the latest communication status.
     */

    g_imuHealthy = state->imuHealthy;
    g_magHealthy = state->magnetometerHealthy;
    g_baroHealthy = state->barometerHealthy;
}

/******************************************************************************
 * Health Status
 ******************************************************************************/

bool GY87_IMU_Healthy(void)
{
    return g_imuHealthy;
}

bool GY87_Magnetometer_Healthy(void)
{
    return g_magHealthy;
}

bool GY87_Barometer_Healthy(void)
{
    return g_baroHealthy;
}

/******************************************************************************
 * End of File
 ******************************************************************************/

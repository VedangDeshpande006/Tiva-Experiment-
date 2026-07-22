#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#include "GY87.h"
#include "config.h"
#include "I2C_Utils.h"

#include "driverlib/sysctl.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

//==================================================
// MPU6050 Registers
//==================================================
#define MPU6050_SMPLRT_DIV        0x19
#define MPU6050_CONFIG            0x1A
#define MPU6050_GYRO_CONFIG       0x1B
#define MPU6050_ACCEL_CONFIG      0x1C
#define MPU6050_INT_PIN_CFG       0x37
#define MPU6050_ACCEL_XOUT_H      0x3B
#define MPU6050_USER_CTRL         0x6A
#define MPU6050_PWR_MGMT_1        0x6B
#define MPU6050_WHO_AM_I          0x75
#define MPU6050_WHO_AM_I_VALUE    0x68

//==================================================
// HMC5883L Registers
//==================================================
#define HMC5883L_REG_CONFIG_A     0x00
#define HMC5883L_REG_MODE         0x02
#define HMC5883L_REG_DATA_START   0x03
#define HMC5883L_REG_IDA          0x0A
#define HMC5883L_IDA_VALUE        0x48

#define HMC5883L_CONFIG_A_VALUE   0x70
#define HMC5883L_CONTINUOUS_MODE  0x00

//==================================================
// Constants
//==================================================
#define MPU6050_TEMP_SENSITIVITY 340.0f
#define MPU6050_TEMP_OFFSET      36.53f

#define STARTUP_DELAY() \
    SysCtlDelay((SYSTEM_CLOCK_HZ / 3000U) * SENSOR_STARTUP_DELAY_MS)

//==================================================
// Helper Functions
//==================================================

static inline int16_t ReadInt16(uint8_t msb, uint8_t lsb)
{
    return (int16_t)((msb << 8) | lsb);
}

static bool GY87_EnableBypassMode(void)
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

static float ComputeHeading(int16_t mx, int16_t my)
{
    float heading = atan2f((float)my, (float)mx);

    if (heading < 0.0f)
    {
        heading += 2.0f * M_PI;
    }

    return heading;
}

//==================================================
// Initialization
//==================================================

bool GY87_Init(void)
{
    uint8_t id;

    if (!I2C_ReadBurst(MPU6050_I2C_ADDRESS,
                       MPU6050_WHO_AM_I,
                       &id,
                       1))
    {
        return false;
    }

    if (id != MPU6050_WHO_AM_I_VALUE)
    {
        return false;
    }

    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_PWR_MGMT_1,
                       0x00))
    {
        return false;
    }

    STARTUP_DELAY();

    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_SMPLRT_DIV,
                       MPU6050_SMPLRT_250HZ))
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

    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_CONFIG,
                       MPU6050_DLPF_CFG))
    {
        return false;
    }

    if (!GY87_EnableBypassMode())
    {
        return false;
    }

    if (!I2C_ReadBurst(HMC5883L_I2C_ADDRESS,
                       HMC5883L_REG_IDA,
                       &id,
                       1))
    {
        return false;
    }

    if (id != HMC5883L_IDA_VALUE)
    {
        return false;
    }

    if (!I2C_WriteByte(HMC5883L_I2C_ADDRESS,
                       HMC5883L_REG_CONFIG_A,
                       HMC5883L_CONFIG_A_VALUE))
    {
        return false;
    }

    if (!I2C_WriteByte(HMC5883L_I2C_ADDRESS,
                       HMC5883L_REG_MODE,
                       HMC5883L_CONTINUOUS_MODE))
    {
        return false;
    }

    return true;
}

//==================================================
// Read Sensor Data
//==================================================

void GY87_UpdateState(GY87_State_t *state)
{
    if (state == NULL)
    {
        return;
    }

    uint8_t imu[14];
    uint8_t mag[6];

    if (!I2C_ReadBurst(MPU6050_I2C_ADDRESS,
                       MPU6050_ACCEL_XOUT_H,
                       imu,
                       14))
    {
        return;
    }

    state->imu.ax = ReadInt16(imu[0], imu[1]);
    state->imu.ay = ReadInt16(imu[2], imu[3]);
    state->imu.az = ReadInt16(imu[4], imu[5]);

    int16_t rawTemp = ReadInt16(imu[6], imu[7]);

    state->imu.temperature =
        ((float)rawTemp / MPU6050_TEMP_SENSITIVITY)
        + MPU6050_TEMP_OFFSET;

    state->imu.gx = ReadInt16(imu[8], imu[9]);
    state->imu.gy = ReadInt16(imu[10], imu[11]);
    state->imu.gz = ReadInt16(imu[12], imu[13]);

    if (!I2C_ReadBurst(HMC5883L_I2C_ADDRESS,
                       HMC5883L_REG_DATA_START,
                       mag,
                       6))
    {
        return;
    }

    // HMC5883L order = X Z Y

    state->magnetometer.mx = ReadInt16(mag[0], mag[1]);
    state->magnetometer.mz = ReadInt16(mag[2], mag[3]);
    state->magnetometer.my = ReadInt16(mag[4], mag[5]);

    state->magnetometer.heading =
        ComputeHeading(state->magnetometer.mx,
                       state->magnetometer.my);
}

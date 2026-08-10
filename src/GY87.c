/*
 * ============================================================================
 * File    : GY87.c
 * Target  : TM4C123GH6PM (Tiva C LaunchPad)
 * Purpose : GY-87 sensor driver
 *
 * Sensors:
 *   - MPU6050 IMU
 *   - HMC5883L magnetometer
 *   - BMP180 detection only (Phase-2 reserved)
 *
 * MPU6050 configuration:
 *   - Gyroscope : +/-2000 deg/s
 *   - Accelerometer : +/-8 g
 *
 * ============================================================================
 */

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

/* ============================================================================
 * MPU6050 Registers
 * ========================================================================== */

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

/* ============================================================================
 * HMC5883L Registers
 * ========================================================================== */

#define HMC5883L_CONFIG_A         0x00
#define HMC5883L_CONFIG_B         0x01
#define HMC5883L_MODE              0x02
#define HMC5883L_DATA_START        0x03

#define HMC5883L_ID_A             0x0A
#define HMC5883L_ID_B             0x0B
#define HMC5883L_ID_C             0x0C

/* HMC5883L Identification Bytes: "H43" */
#define HMC5883L_ID_A_VALUE       0x48
#define HMC5883L_ID_B_VALUE       0x34
#define HMC5883L_ID_C_VALUE       0x33

/* HMC5883L Configuration Values */
#define HMC5883L_CONFIG_A_VALUE   0x70
#define HMC5883L_CONFIG_B_VALUE   0x20
#define HMC5883L_CONTINUOUS_MODE  0x00

/* Default HMC5883L sensitivity:
 * +/-1.3 Gauss -> 1090 LSB/Gauss
 */
#define HMC5883L_SENSITIVITY      1090.0f

/* ============================================================================
 * BMP180 Registers
 * ========================================================================== */

#define BMP180_CHIP_ID            0xD0
#define BMP180_CHIP_VALUE         0x55

/* ============================================================================
 * MPU6050 Temperature Conversion
 * ========================================================================== */

#define MPU6050_TEMP_SENSITIVITY  340.0f
#define MPU6050_TEMP_OFFSET       36.53f

/* ============================================================================
 * Driver State
 * ========================================================================== */

static bool g_imuHealthy  = false;
static bool g_magHealthy  = false;
static bool g_baroHealthy = false;

/* ============================================================================
 * Local Helpers
 * ========================================================================== */

static int16_t GY87_CombineBytes(uint8_t msb, uint8_t lsb)
{
    return (int16_t)(((uint16_t)msb << 8) | (uint16_t)lsb);
}

static float GY87_ComputeHeading(float mx, float my)
{
    float heading;

    heading = atan2f(my, mx);

    if (heading < 0.0f)
    {
        heading += (2.0f * M_PI);
    }

    return heading;
}

/* ============================================================================
 * MPU6050 I2C Bypass
 * ========================================================================== */

static bool MPU6050_EnableBypass(void)
{
    /*
     * Disable MPU6050 master I2C mode.
     */
    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_USER_CTRL,
                       0x00))
    {
        return false;
    }

    /*
     * Enable I2C bypass so the TM4C123 can directly access
     * the HMC5883L connected behind the MPU6050.
     */
    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_INT_PIN_CFG,
                       0x02))
    {
        return false;
    }

    return true;
}

/* ============================================================================
 * MPU6050 Initialization
 * ========================================================================== */

static bool MPU6050_Init(void)
{
    uint8_t id = 0U;

    /*
     * Verify MPU6050 identity.
     */
    if (!I2C_ReadBurst(MPU6050_I2C_ADDRESS,
                       MPU6050_WHO_AM_I,
                       &id,
                       1U))
    {
        return false;
    }

    if (id != MPU6050_DEVICE_ID)
    {
        return false;
    }

    /*
     * Wake MPU6050.
     */
    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_PWR_MGMT_1,
                       0x00))
    {
        return false;
    }

    /*
     * Allow sensor startup.
     */
    SysCtlDelay((SYSTEM_CLOCK_HZ / 3000U) *
                SENSOR_STARTUP_DELAY_MS);

    /*
     * Sample-rate divider.
     */
    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_SMPLRT_DIV,
                       MPU6050_SMPLRT_DIV_VALUE))
    {
        return false;
    }

    /*
     * Digital low-pass filter.
     */
    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_CONFIG,
                       MPU6050_DLPF_CFG))
    {
        return false;
    }

    /*
     * Gyroscope full-scale range:
     * +/-2000 deg/s.
     */
    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_GYRO_CONFIG,
                       MPU6050_GYRO_FS_2000))
    {
        return false;
    }

    /*
     * Accelerometer full-scale range:
     * +/-8 g.
     */
    if (!I2C_WriteByte(MPU6050_I2C_ADDRESS,
                       MPU6050_ACCEL_CONFIG,
                       MPU6050_ACCEL_FS_8))
    {
        return false;
    }

    /*
     * Enable direct I2C access to HMC5883L.
     */
    if (!MPU6050_EnableBypass())
    {
        return false;
    }

    return true;
}

/* ============================================================================
 * HMC5883L Initialization
 * ========================================================================== */

static bool HMC5883L_Init(void)
{
    uint8_t id[3];

    /*
     * HMC5883L identification registers contain:
     *
     *   0x0A = 'H' = 0x48
     *   0x0B = '4' = 0x34
     *   0x0C = '3' = 0x33
     */
    if (!I2C_ReadBurst(HMC5883L_I2C_ADDRESS,
                       HMC5883L_ID_A,
                       id,
                       3U))
    {
        return false;
    }

    if ((id[0] != HMC5883L_ID_A_VALUE) ||
        (id[1] != HMC5883L_ID_B_VALUE) ||
        (id[2] != HMC5883L_ID_C_VALUE))
    {
        return false;
    }

    /*
     * Configuration Register A:
     *   8-average
     *   15 Hz measurement output rate
     *   Normal measurement mode
     */
    if (!I2C_WriteByte(HMC5883L_I2C_ADDRESS,
                       HMC5883L_CONFIG_A,
                       HMC5883L_CONFIG_A_VALUE))
    {
        return false;
    }

    /*
     * Configuration Register B:
     *   +/-1.3 Gauss range
     */
    if (!I2C_WriteByte(HMC5883L_I2C_ADDRESS,
                       HMC5883L_CONFIG_B,
                       HMC5883L_CONFIG_B_VALUE))
    {
        return false;
    }

    /*
     * Continuous measurement mode.
     */
    if (!I2C_WriteByte(HMC5883L_I2C_ADDRESS,
                       HMC5883L_MODE,
                       HMC5883L_CONTINUOUS_MODE))
    {
        return false;
    }

    return true;
}

/* ============================================================================
 * BMP180 Detection
 * ========================================================================== */

static bool BMP180_Init(void)
{
    uint8_t id = 0U;

    if (!I2C_ReadBurst(BMP180_I2C_ADDRESS,
                       BMP180_CHIP_ID,
                       &id,
                       1U))
    {
        return false;
    }

    return (id == BMP180_CHIP_VALUE);
}

/* ============================================================================
 * Public Initialization
 * ========================================================================== */

bool GY87_Init(void)
{
    /*
     * MPU6050 is mandatory for flight.
     */
    g_imuHealthy = MPU6050_Init();

    g_magHealthy  = false;
    g_baroHealthy = false;

    /*
     * Magnetometer is initialized only if the MPU6050
     * is successfully initialized and I2C bypass is available.
     */
    if (g_imuHealthy)
    {
        g_magHealthy = HMC5883L_Init();
    }

    /*
     * BMP180 is only detected in Phase-1.
     * Full barometer operation is reserved for Phase-2.
     */
    g_baroHealthy = BMP180_Init();

    /*
     * Flight controller can operate with MPU6050 alone.
     */
    return g_imuHealthy;
}

/* ============================================================================
 * Read MPU6050 IMU
 * ========================================================================== */

bool GY87_ReadIMU(GY87_IMU_t *imu)
{
    uint8_t buffer[14];
    int16_t rawTemp;

    if (imu == NULL)
    {
        return false;
    }

    /*
     * Read:
     *
     *   ACCEL_XOUT_H ... ACCEL_ZOUT_L
     *   TEMP_OUT_H   ... TEMP_OUT_L
     *   GYRO_XOUT_H  ... GYRO_ZOUT_L
     *
     * Total = 14 bytes.
     */
    if (!I2C_ReadBurst(MPU6050_I2C_ADDRESS,
                       MPU6050_ACCEL_XOUT_H,
                       buffer,
                       sizeof(buffer)))
    {
        return false;
    }

    /*
     * Accelerometer raw values.
     */
    imu->ax = GY87_CombineBytes(buffer[0], buffer[1]);
    imu->ay = GY87_CombineBytes(buffer[2], buffer[3]);
    imu->az = GY87_CombineBytes(buffer[4], buffer[5]);

    /*
     * Temperature.
     */
    rawTemp = GY87_CombineBytes(buffer[6], buffer[7]);

    imu->temperature =
        ((float)rawTemp / MPU6050_TEMP_SENSITIVITY) +
        MPU6050_TEMP_OFFSET;

    /*
     * Gyroscope raw values.
     */
    imu->gx = GY87_CombineBytes(buffer[8],  buffer[9]);
    imu->gy = GY87_CombineBytes(buffer[10], buffer[11]);
    imu->gz = GY87_CombineBytes(buffer[12], buffer[13]);

    /*
     * Convert raw accelerometer values to g.
     *
     * ACCEL_SCALE must match the configured +/-8 g range.
     */
    imu->accelX_g = (float)imu->ax * ACCEL_SCALE;
    imu->accelY_g = (float)imu->ay * ACCEL_SCALE;
    imu->accelZ_g = (float)imu->az * ACCEL_SCALE;

    /*
     * Convert raw gyro values to degrees/second.
     *
     * GYRO_SCALE must match the configured +/-2000 deg/s range.
     */
    imu->gyroX_dps = (float)imu->gx * GYRO_SCALE;
    imu->gyroY_dps = (float)imu->gy * GYRO_SCALE;
    imu->gyroZ_dps = (float)imu->gz * GYRO_SCALE;

    return true;
}

/* ============================================================================
 * Read HMC5883L Magnetometer
 * ========================================================================== */

bool GY87_ReadMagnetometer(GY87_Magnetometer_t *mag)
{
    uint8_t buffer[6];

    if (mag == NULL)
    {
        return false;
    }

    /*
     * HMC5883L output order:
     *
     *   X high/low
     *   Z high/low
     *   Y high/low
     */
    if (!I2C_ReadBurst(HMC5883L_I2C_ADDRESS,
                       HMC5883L_DATA_START,
                       buffer,
                       sizeof(buffer)))
    {
        return false;
    }

    mag->mx = GY87_CombineBytes(buffer[0], buffer[1]);
    mag->mz = GY87_CombineBytes(buffer[2], buffer[3]);
    mag->my = GY87_CombineBytes(buffer[4], buffer[5]);

    /*
     * Convert to Gauss.
     */
    mag->fieldX = (float)mag->mx / HMC5883L_SENSITIVITY;
    mag->fieldY = (float)mag->my / HMC5883L_SENSITIVITY;
    mag->fieldZ = (float)mag->mz / HMC5883L_SENSITIVITY;

    /*
     * Calculate raw magnetic heading.
     *
     * This is not tilt compensated.
     */
    mag->heading = GY87_ComputeHeading(
        mag->fieldX,
        mag->fieldY);

    return true;
}

/* ============================================================================
 * Read BMP180
 *
 * Phase-1 placeholder.
 * ========================================================================== */

bool GY87_ReadBarometer(GY87_Barometer_t *baro)
{
    if (baro == NULL)
    {
        return false;
    }

    /*
     * Phase-2 implementation will include:
     *
     *   - Calibration EEPROM read
     *   - Uncompensated temperature
     *   - Uncompensated pressure
     *   - Compensated pressure
     *   - Altitude calculation
     */

    baro->initialized = g_baroHealthy;

    baro->rawPressure = 0;
    baro->rawTemperature = 0;

    baro->pressure_hPa = 0.0f;
    baro->temperature = 0.0f;
    baro->altitude_m = 0.0f;

    return g_baroHealthy;
}

/* ============================================================================
 * Update Complete Sensor State
 * ========================================================================== */

void GY87_UpdateState(GY87_State_t *state)
{
    if (state == NULL)
    {
        return;
    }

    /*
     * Read mandatory IMU.
     */
    state->imuHealthy =
        GY87_ReadIMU(&state->imu);

    /*
     * Read magnetometer.
     */
    state->magnetometerHealthy =
        GY87_ReadMagnetometer(&state->magnetometer);

    /*
     * Read/detect barometer.
     */
    state->barometerHealthy =
        GY87_ReadBarometer(&state->barometer);

    /*
     * Keep module health flags synchronized
     * with the latest communication results.
     */
    g_imuHealthy  = state->imuHealthy;
    g_magHealthy  = state->magnetometerHealthy;
    g_baroHealthy = state->barometerHealthy;
}

/* ============================================================================
 * Health Status
 * ========================================================================== */

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

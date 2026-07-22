#include <math.h>
#include "GY87.h"
#include "config.h"
#include "I2C_Utils.h"

// HMC5883L Magnetometer Direct Register Definitions
#define HMC5883L_REG_CONFIG_A   0x00
#define HMC5883L_REG_MODE       0x02
#define HMC5883L_REG_DATA_START 0x03

// MPU6050 Temperature Conversion Constants
#define MPU6050_TEMP_SENSITIVITY 340.0f
#define MPU6050_TEMP_OFFSET      36.53f

// Internal function to break the MPU6050 master lock and bridge the I2C lines
static void GY87_EnableBypassMode(void) {
    // Explicitly pass MPU6050_I2C_ADDRESS to unlock external sub-bus access
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_USER_CTRL, 0x00);
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_INT_PIN_CFG, 0x02);
}

void GY87_Init(void) {
    // 1. Wake up MPU6050 from default internal sleep state
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_PWR_MGMT_1, 0x00);
    
    // 2. Set DLPF (Digital Low Pass Filter) to reject high-frequency frame vibrations
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_CONFIG, MPU6050_DLPF_CFG);
    
    // 3. Enable Bypass so the Tiva C master can directly address the Mag and Baro
    GY87_EnableBypassMode();
    
    // 4. Initialize HMC5883L Magnetometer
    // Config A: 8-sample average, 15Hz default update rate -> 0x70
    I2C_WriteByte(HMC5883L_I2C_ADDRESS, HMC5883L_REG_CONFIG_A, 0x70);
    // Mode Reg: Set to Continuous Measurement Mode -> 0x00
    I2C_WriteByte(HMC5883L_I2C_ADDRESS, HMC5883L_REG_MODE, 0x00);
    
    // NOTE: BMP180 non-blocking initialization sequence will be added here next!
}

void GY87_UpdateState(GY87_State_t* state) {
    uint8_t imuBuffer[14];
    uint8_t magBuffer[6];
    
    // ========================================================================
    // 1. READ MPU6050 IMU DATA
    // ========================================================================
    I2C_ReadBurst(MPU6050_I2C_ADDRESS, MPU6050_ACCEL_XOUT_H, imuBuffer, 14);
    
    // Parse Accelerometer Raw Values
    state->imu.ax = (int16_t)((imuBuffer[0] << 8)  | imuBuffer[1]);
    state->imu.ay = (int16_t)((imuBuffer[2] << 8)  | imuBuffer[3]);
    state->imu.az = (int16_t)((imuBuffer[4] << 8)  | imuBuffer[5]);
    
    // Parse Internal Die Temperature (Useful for thermal drift checking)
    int16_t rawTemp = (int16_t)((imuBuffer[6] << 8) | imuBuffer[7]);
    state->imu.temperature = ((float)rawTemp / MPU6050_TEMP_SENSITIVITY) + MPU6050_TEMP_OFFSET;
    
    // Parse Gyroscope Raw Values
    state->imu.gx = (int16_t)((imuBuffer[8] << 8)  | imuBuffer[9]);
    state->imu.gy = (int16_t)((imuBuffer[10] << 8) | imuBuffer[11]);
    state->imu.gz = (int16_t)((imuBuffer[12] << 8) | imuBuffer[13]);
    
    // ========================================================================
    // 2. READ HMC5883L MAGNETOMETER DATA
    // ========================================================================
    // Register 0x03 is the start of the Data Output Registers (X MSB)
    I2C_ReadBurst(HMC5883L_I2C_ADDRESS, HMC5883L_REG_DATA_START, magBuffer, 6);
    
    // Note: HMC5883L registers store data in a distinct sequence: X, Z, Y
    state->magnetometer.mx = (int16_t)((magBuffer[0] << 8) | magBuffer[1]);
    state->magnetometer.mz = (int16_t)((magBuffer[2] << 8) | magBuffer[3]);
    state->magnetometer.my = (int16_t)((magBuffer[4] << 8) | magBuffer[5]);
    
    // Calculate simple raw layout heading (in radians)
    state->magnetometer.heading = atan2f((float)state->magnetometer.my, (float)state->magnetometer.mx);
    
    // ========================================================================
    // 3. READ BMP180 BAROMETER DATA
    // ========================================================================
    // TODO: Add non-blocking BMP180 state-machine read here in the next step
}

#include <math.h>
#include "GY87.h"
#include "config.h"

// Internal function to break the MPU6050 master lock and bridge the I2C lines
static void GY87_EnableBypassMode(void) {
    // We now explicitly pass MPU6050_I2C_ADDRESS as the first parameter
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_USER_CTRL, 0x00);
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_INT_PIN_CFG, 0x02);
}

void GY87_Init(void) {
    // 1. Wake up MPU6050 (Pass address explicitly)
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_PWR_MGMT_1, 0x00);
    
    // 2. Set DLPF (Digital Low Pass Filter) to reject frame vibrations
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_CONFIG, MPU6050_DLPF_CFG);
    
    // 3. Enable Bypass so the Tiva C can directly address the Magnetometer and Barometer
    GY87_EnableBypassMode();
    
    // 4. Initialize HMC5883L Magnetometer
    // Set configuration register A (0x00) to 8-sample average, 15Hz default -> 0x70
    I2C_WriteByte(HMC5883L_I2C_ADDRESS, 0x00, 0x70);
    // Set Mode register (0x02) to Continuous Measurement Mode -> 0x00
    I2C_WriteByte(HMC5883L_I2C_ADDRESS, 0x02, 0x00);
    
    // NOTE: BMP180 initialization sequence will be added here next!
}

void GY87_UpdateState(GY87_State_t* state) {
    uint8_t imuBuffer[14];
    uint8_t magBuffer[6];
    
    // ==========================================
    // 1. READ MPU6050 IMU DATA
    // ==========================================
    I2C_ReadBurst(MPU6050_I2C_ADDRESS, MPU6050_ACCEL_XOUT_H, imuBuffer, 14);
    
    state->imu.ax = (int16_t)((imuBuffer[0] << 8)  | imuBuffer[1]);
    state->imu.ay = (int16_t)((imuBuffer[2] << 8)  | imuBuffer[3]);
    state->imu.az = (int16_t)((imuBuffer[4] << 8)  | imuBuffer[5]);
    
    state->imu.gx = (int16_t)((imuBuffer[8] << 8)  | imuBuffer[9]);
    state->imu.gy = (int16_t)((imuBuffer[10] << 8) | imuBuffer[11]);
    state->imu.gz = (int16_t)((imuBuffer[12] << 8) | imuBuffer[13]);
    
    // ==========================================
    // 2. READ HMC5883L MAGNETOMETER DATA
    // ==========================================
    // Register 0x03 is the start of the Data Output Registers (X MSB)
    I2C_ReadBurst(HMC5883L_I2C_ADDRESS, 0x03, magBuffer, 6);
    
    // Note: HMC5883L registers store data in the sequence: X, Z, Y
    state->magnetometer.mx = (int16_t)((magBuffer[0] << 8) | magBuffer[1]);
    state->magnetometer.mz = (int16_t)((magBuffer[2] << 8) | magBuffer[3]);
    state->magnetometer.my = (int16_t)((magBuffer[4] << 8) | magBuffer[5]);
    
    // Calculate simple raw layout heading (in radians)
    state->magnetometer.heading = atan2f((float)state->magnetometer.my, (float)state->magnetometer.mx);
    
    // ==========================================
    // 3. READ BMP180 BAROMETER DATA
    // ==========================================
    // TODO: Add non-blocking BMP180 state-machine read here in the next step
}

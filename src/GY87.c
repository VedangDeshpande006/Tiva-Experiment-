#include <math.h>[cite: 6]
#include "GY87.h"
#include "config.h"[cite: 6]

// Internal function to configure the bypass for HMC5883L
static void GY87_EnableBypassMode(void) {
    // Disable MPU6050 Master Mode
    I2C_WriteByte(MPU6050_USER_CTRL, 0x00);
    // Enable I2C Bypass En bit 
    I2C_WriteByte(MPU6050_INT_PIN_CFG, 0x02);
}

void GY87_Init(void) {
    // 1. Wake up MPU6050
    I2C_WriteByte(MPU6050_PWR_MGMT_1, 0x00);
    
    // 2. Set DLPF (Digital Low Pass Filter)
    I2C_WriteByte(MPU6050_CONFIG, MPU6050_DLPF_CFG);
    
    // 3. Enable Bypass to see the HMC5883L on the I2C bus
    GY87_EnableBypassMode();
    
    // TODO: Add HMC5883L and BMP180 init sequences here in Stage 3
}

void GY87_UpdateState(GY87_State_t* state) {
    uint8_t rawBuffer[14];[cite: 6]
    
    // Pull the 14-byte burst from the sensor[cite: 6]
    I2C_ReadBurst(MPU6050_ACCEL_XOUT_H, rawBuffer, 14);[cite: 6]
    
    // Populate the IMU structure[cite: 6]
    state->imu.ax = (int16_t)((rawBuffer[0] << 8)  | rawBuffer[1]);[cite: 6]
    state->imu.ay = (int16_t)((rawBuffer[2] << 8)  | rawBuffer[3]);[cite: 6]
    state->imu.az = (int16_t)((rawBuffer[4] << 8)  | rawBuffer[5]);[cite: 6]
    
    state->imu.gx = (int16_t)((rawBuffer[8] << 8)  | rawBuffer[9]);[cite: 6]
    state->imu.gy = (int16_t)((rawBuffer[10] << 8) | rawBuffer[11]);[cite: 6]
    state->imu.gz = (int16_t)((rawBuffer[12] << 8) | rawBuffer[13]);[cite: 6]
    
    // TODO: Add HMC5883L and BMP180 data reads here
}

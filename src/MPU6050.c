#include "MPU6050.h"
#include "Config.h" // Pulls MPU6050_DLPF_CFG definition

void MPU6050_Init(void) {
    // Reset and Wake up device (Internal clock source 8MHz)
    I2C_WriteByte(MPU6050_PWR_MGMT_1, 0x00);
    
    // Set Digital Low Pass Filter (DLPF) to smooth out structural frame noises
    I2C_WriteByte(MPU6050_CONFIG, MPU6050_DLPF_CFG);
    
    // Set Sample Rate Divider: Sample Rate = 1kHz / (1 + 3) = 250Hz (matches loop time)
    I2C_WriteByte(MPU6050_SMPLRT_DIV, 0x03);
    
    // Configure Gyro Full-Scale Range (+/- 2000 deg/s)
    I2C_WriteByte(MPU6050_GYRO_CONFIG, 0x18);
    
    // Configure Accelerometer Full-Scale Range (+/- 2g)
    I2C_WriteByte(MPU6050_ACCEL_CONFIG, 0x00);
}

void MPU6050_ReadRaw(int16_t *accelX, int16_t *accelY, int16_t *accelZ,
                     int16_t *gyroX,  int16_t *gyroY,  int16_t *gyroZ) {
    uint8_t rawBuffer[14];
    
    // Extract 14 sequential bytes (AccX, AccY, AccZ, Temp, GyroX, GyroY, GyroZ)
    I2C_ReadBurst(MPU6050_ACCEL_XOUT_H, rawBuffer, 14);
    
    // Shift and reconstruct raw high/low bytes into 16-bit signed variables
    *accelX = (int16_t)((rawBuffer[0] << 8)  | rawBuffer[1]);
    *accelY = (int16_t)((rawBuffer[2] << 8)  | rawBuffer[3]);
    *accelZ = (int16_t)((rawBuffer[4] << 8)  | rawBuffer[5]);
    
    // Bytes 6-7 are internal die temperature, skipping for main dynamics calculation
    
    *gyroX  = (int16_t)((rawBuffer[8] << 8)  | rawBuffer[9]);
    *gyroY  = (int16_t)((rawBuffer[10] << 8) | rawBuffer[11]);
    *gyroZ  = (int16_t)((rawBuffer[12] << 8) | rawBuffer[13]);
}

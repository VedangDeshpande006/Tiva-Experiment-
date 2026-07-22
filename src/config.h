#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// I2C ADDRESSES
// ==========================================
#define MPU6050_I2C_ADDRESS  0x68
#define HMC5883L_I2C_ADDRESS 0x1E

// ==========================================
// GY-87 TUNING PARAMETERS
// ==========================================
// DLPF (Digital Low Pass Filter) - Set to ~42Hz bandwidth to filter frame vibrations
#define MPU6050_DLPF_CFG     0x03 

// Gyro Config: Full Scale Range = ±2000 deg/s (Bit 3,4 = 11 -> 0x18)
#define MPU6050_GYRO_FS_2000 0x18 

// Accel Config: Full Scale Range = ±8g (Bit 3,4 = 10 -> 0x10)
#define MPU6050_ACCEL_FS_8   0x10 

// Sample Rate Divider: 1kHz internal clock / (1 + 3) = 250Hz sample rate
#define MPU6050_SMPLRT_250HZ 0x03 

// ==========================================
// SENSOR SCALING FACTORS
// ==========================================
// At ±8g, LSB sensitivity is 4096 LSB/g
#define ACCEL_SCALE (1.0f / 4096.0f) 

// At ±2000 deg/s, LSB sensitivity is 16.4 LSB/(deg/s)
#define GYRO_SCALE  (1.0f / 16.4f)   

#endif // CONFIG_H

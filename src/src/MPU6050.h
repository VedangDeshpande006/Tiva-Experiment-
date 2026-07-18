#ifndef _MPU6050_H_
#define _MPU6050_H_

#include <stdint.h>

// MPU6050 I2C Address (From Config.h)
#define MPU6050_ADDRESS       0x68

// Core Registers
#define MPU6050_SMPLRT_DIV    0x19
#define MPU6050_CONFIG        0x1A
#define MPU6050_GYRO_CONFIG   0x1B
#define MPU6050_ACCEL_CONFIG  0x1C
#define MPU6050_PWR_MGMT_1    0x6B
#define MPU6050_WHO_AM_I      0x75

// Raw Data Start Register (Burst reads start here)
#define MPU6050_ACCEL_XOUT_H  0x3B

// Sensor Scaling Factors (For Full-Scale Ranges: Accel +/-2g, Gyro +/-2000 deg/s)
#define ACCEL_SCALE           16384.0f
#define GYRO_SCALE            16.4f

// Function Prototypes
void MPU6050_Init(void);
void MPU6050_ReadRaw(int16_t *accelX, int16_t *accelY, int16_t *accelZ,
                     int16_t *gyroX,  int16_t *gyroY,  int16_t *gyroZ);

#endif /* _MPU6050_H_ */

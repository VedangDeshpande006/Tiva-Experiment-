#ifndef _GY87_H_
#define _GY87_H_

#include <stdint.h>
#include <stdbool.h>

// MPU6050 Core Registers (Absorbed from old MPU6050.h)[cite: 7]
#define MPU6050_SMPLRT_DIV    0x19[cite: 7]
#define MPU6050_CONFIG        0x1A[cite: 7]
#define MPU6050_GYRO_CONFIG   0x1B[cite: 7]
#define MPU6050_ACCEL_CONFIG  0x1C[cite: 7]
#define MPU6050_INT_PIN_CFG   0x37
#define MPU6050_ACCEL_XOUT_H  0x3B // Raw Data Start Register[cite: 7]
#define MPU6050_USER_CTRL     0x6A
#define MPU6050_PWR_MGMT_1    0x6B[cite: 7]

// Sensor Scaling Factors[cite: 7]
#define ACCEL_SCALE           16384.0f[cite: 7]
#define GYRO_SCALE            16.4f[cite: 7]

// Unified Data Structures
typedef struct {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
} MPU6050_Data_t;

typedef struct {
    int16_t mx, my, mz;
    float heading;
} HMC5883L_Data_t;

typedef struct {
    int32_t raw_pressure;
    float altitude_m;
} BMP180_Data_t;

// Consolidated GY-87 Master Abstraction Layer
typedef struct {
    MPU6050_Data_t   imu;
    HMC5883L_Data_t  magnetometer;
    BMP180_Data_t    barometer;
    bool             is_calibrated;
} GY87_State_t;

// Function Prototypes 
void GY87_Init(void);
void GY87_UpdateState(GY87_State_t* state);

// Low-level dependencies (in GY87.h)
extern void I2C_WriteByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t data);
extern void I2C_ReadBurst(uint8_t slaveAddr, uint8_t startRegAddr, uint8_t *buffer, uint32_t length);

#endif /* _GY87_H_ */

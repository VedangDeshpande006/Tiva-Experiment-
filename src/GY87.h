#ifndef GY87_H
#define GY87_H

#include <stdint.h>
#include <stdbool.h>

// ==========================================
// MPU6050 HARDWARE REGISTERS & IDs
// ==========================================
#define MPU6050_SMPLRT_DIV       0x19
#define MPU6050_CONFIG           0x1A
#define MPU6050_GYRO_CONFIG      0x1B
#define MPU6050_ACCEL_CONFIG     0x1C
#define MPU6050_INT_PIN_CFG      0x37
#define MPU6050_ACCEL_XOUT_H     0x3B
#define MPU6050_USER_CTRL        0x6A
#define MPU6050_PWR_MGMT_1       0x6B
#define MPU6050_WHO_AM_I         0x75
#define MPU6050_WHO_AM_I_VALUE   0x68

// ==========================================
// HMC5883L HARDWARE REGISTERS & IDs
// ==========================================
#define HMC5883L_REG_CONFIG_A    0x00
#define HMC5883L_REG_MODE        0x02
#define HMC5883L_REG_DATA_START  0x03
#define HMC5883L_REG_IDA         0x0A 
#define HMC5883L_IDA_VALUE       0x48 

// MPU6050 Raw Data Structure
typedef struct {
    int16_t ax, ay, az;
    float temperature; 
    int16_t gx, gy, gz;
} MPU6050_Data_t;

// HMC5883L Raw Data Structure
typedef struct {
    int16_t mx, my, mz;
    float heading; 
} HMC5883L_Data_t;

// Unified GY-87 State Container
typedef struct {
    MPU6050_Data_t imu;
    HMC5883L_Data_t magnetometer;
    // TODO: Add BMP180_Data_t barometer;
} GY87_State_t;

// Function Prototypes
bool GY87_Init(void);
void GY87_UpdateState(GY87_State_t* state);

#endif // GY87_H

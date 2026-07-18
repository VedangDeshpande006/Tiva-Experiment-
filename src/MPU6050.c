#include <math.h>
#include "MPU6050.h"
#include "Config.h"
#include "FlightState.h"

FlightDynamics_t g_droneState; // Instantiate the global state

// (Keep the MPU6050_Init function exactly as it was)

// New, flexible update function
void MPU6050_UpdateState(void) {
    uint8_t rawBuffer[14];
    
    // 1. Pull the 14-byte burst from the sensor
    I2C_ReadBurst(MPU6050_ACCEL_XOUT_H, rawBuffer, 14);
    
    // 2. Populate the raw fields in our state structure
    g_droneState.rawAccX = (int16_t)((rawBuffer[0] << 8)  | rawBuffer[1]);
    g_droneState.rawAccY = (int16_t)((rawBuffer[2] << 8)  | rawBuffer[3]);
    g_droneState.rawAccZ = (int16_t)((rawBuffer[4] << 8)  | rawBuffer[5]);
    
    g_droneState.rawGyroX  = (int16_t)((rawBuffer[8] << 8)  | rawBuffer[9]);
    g_droneState.rawGyroY  = (int16_t)((rawBuffer[10] << 8) | rawBuffer[11]);
    g_droneState.rawGyroZ  = (int16_t)((rawBuffer[12] << 8) | rawBuffer[13]);
    
    // 3. Pre-calculate the geometry right here to keep main.c clean
    float ax = (float)g_droneState.rawAccX / ACCEL_SCALE;
    float ay = (float)g_droneState.rawAccY / ACCEL_SCALE;
    float az = (float)g_droneState.rawAccZ / ACCEL_SCALE;
    
    g_droneState.gyroRateX = (float)g_droneState.rawGyroX / GYRO_SCALE;
    g_droneState.gyroRateY = (float)g_droneState.rawGyroY / GYRO_SCALE;
    g_droneState.gyroRateZ = (float)g_droneState.rawGyroZ / GYRO_SCALE;
    
    // Calculate raw accelerometer pitch and roll
    g_droneState.accRoll  = atan2f(ay, az) * 57.29578f;
    g_droneState.accPitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 57.29578f;
}

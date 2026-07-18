#include <math.h>
#include "MPU6050.h"
#include "Kalman.h"

// Define Kalman Structures for Pitch and Roll
Kalman_t kalmanRoll;
Kalman_t kalmanPitch;

void FlightController_SensorProcess(float dt) {
    int16_t ax_raw, ay_raw, az_raw;
    int16_t gx_raw, gy_raw, gz_raw;
    
    // 1. Pull current hardware data packets
    MPU6050_ReadRaw(&ax_raw, &ay_raw, &az_raw, &gx_raw, &gy_raw, &gz_raw);
    
    // 2. Convert to real physical scales
    float ax = (float)ax_raw / ACCEL_SCALE;
    float ay = (float)ay_raw / ACCEL_SCALE;
    float az = (float)az_raw / ACCEL_SCALE;
    
    // Convert Gyro to degrees per second (deg/s)
    float gyroXRate = (float)gx_raw / GYRO_SCALE;
    float gyroYRate = (float)gy_raw / GYRO_SCALE;
    
    // 3. Compute pitch and roll directly from raw acceleration geometry
    // atan2 outputs radians, multiplying by 57.29578f converts to degrees
    float accRoll  = atan2f(ay, az) * 57.29578f;
    float accPitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 57.29578f;
    
    // 4. Run the data state step through the Kalman Filter
    // Invert the pitch rate to remain consistent with aircraft principle tracking
    float cleanRoll  = Kalman_getAngle(&kalmanRoll, accRoll, gyroXRate, dt);
    float cleanPitch = Kalman_getAngle(&kalmanPitch, accPitch, -gyroYRate, dt);
    
    // cleanRoll and cleanPitch are now lag-free, perfectly stable target coordinates 
    // ready to be fed directly into your PID error calculations!
}

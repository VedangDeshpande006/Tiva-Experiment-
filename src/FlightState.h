#ifndef _FLIGHTSTATE_H_
#define _FLIGHTSTATE_H_

#include <stdint.h>

// A universal container for the quadcopter's physical orientation
typedef struct {
    // Raw Sensor Inputs
    int16_t rawAccX, rawAccY, rawAccZ;
    int16_t rawGyroX, rawGyroY, rawGyroZ;
    
    // Converted Physical Values (G-forces and Degrees/sec)
    float accRoll, accPitch;
    float gyroRateX, gyroRateY, gyroRateZ;
    
    // Final Clean Angles (Output from Kalman Filter)
    float roll;
    float pitch;
    float yaw;
    
} FlightDynamics_t;

// Global instance that the whole program can read
extern FlightDynamics_t g_droneState;

#endif /* _FLIGHTSTATE_H_ */

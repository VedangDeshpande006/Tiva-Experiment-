#ifndef _FLIGHTSTATE_H_
#define _FLIGHTSTATE_H_

#include <stdint.h>
#include "GY87.h"

// A universal container for the quadcopter's physical orientation[cite: 3]
typedef struct {
    // Consolidated 10-DOF Sensor State
    GY87_State_t sensors;
    
    // Converted Physical Values (G-forces and Degrees/sec)[cite: 3]
    float accRoll, accPitch;[cite: 3]
    float gyroRateX, gyroRateY, gyroRateZ;[cite: 3]
    
    // Final Clean Angles (Output from Kalman Filter)[cite: 3]
    float roll;[cite: 3]
    float pitch;[cite: 3]
    float yaw;[cite: 3]
    
} FlightDynamics_t;[cite: 3]

// Global instance that the whole program can read[cite: 3]
extern FlightDynamics_t g_droneState;[cite: 3]

#endif /* _FLIGHTSTATE_H_ */

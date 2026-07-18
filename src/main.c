#include "FlightState.h"
#include "MPU6050.h"
#include "Kalman.h"

Kalman_t kalmanRoll;
Kalman_t kalmanPitch;

void FlightController_SensorProcess(float dt) {
    // 1. Tell the sensor driver to update the global drone state
    MPU6050_UpdateState();
    
    // 2. Feed the freshly updated drone state into Lauszus's Kalman math
    g_droneState.roll = Kalman_getAngle(&kalmanRoll, g_droneState.accRoll, g_droneState.gyroRateX, dt);
    
    g_droneState.pitch = Kalman_getAngle(&kalmanPitch, g_droneState.accPitch, -g_droneState.gyroRateY, dt);
    
    // Now g_droneState.roll and g_droneState.pitch are perfectly clean 
    // and ready to be compared against your XBee Xbox commands!
}

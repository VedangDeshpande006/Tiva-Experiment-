#ifndef FLIGHTSTATE_H
#define FLIGHTSTATE_H

#include <stdint.h>
#include <stdbool.h>

//==================================================
// ATTITUDE STATE
//==================================================
typedef struct
{
    float roll;
    float pitch;
    float yaw;

    // Sensor calibration offsets (raw LSB)
    float gyroBiasX;
    float gyroBiasY;
    float gyroBiasZ;

    float accelBiasX;
    float accelBiasY;
    float accelBiasZ;

} Attitude_t;


//==================================================
// RC COMMAND SETPOINTS
//==================================================
typedef struct
{
    float targetRoll;
    float targetPitch;
    float targetYawRate;

    uint16_t throttle;

} Setpoint_t;


//==================================================
// MOTOR OUTPUTS
//==================================================
typedef struct
{
    uint16_t m1;
    uint16_t m2;
    uint16_t m3;
    uint16_t m4;

} MotorMix_t;


//==================================================
// FLIGHT MODES
//==================================================
typedef enum
{
    SYSTEM_INIT = 0,
    SYSTEM_CALIBRATING,
    SYSTEM_DISARMED,
    SYSTEM_ARMED,
    SYSTEM_FAILSAFE

} SystemMode_t;


//==================================================
// COMPLETE FLIGHT STATE
//==================================================
typedef struct
{
    SystemMode_t mode;

    Attitude_t attitude;

    Setpoint_t setpoint;

    MotorMix_t motors;

} FlightState_t;


//==================================================
// PUBLIC FUNCTIONS
//==================================================

void FlightState_Init(FlightState_t *state);

bool FlightState_CalibrateSensors(FlightState_t *state);

#endif

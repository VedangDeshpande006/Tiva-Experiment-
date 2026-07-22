#ifndef FLIGHTSTATE_H
#define FLIGHTSTATE_H

#include <stdint.h>
#include <stdbool.h>

#include "GY87.h"

//==================================================
// ATTITUDE ESTIMATION
//==================================================

typedef struct
{
    // Estimated Euler Angles (degrees)
    float roll;
    float pitch;
    float yaw;

    // Sensor Calibration Offsets
    float gyroBiasX;
    float gyroBiasY;
    float gyroBiasZ;

    float accelBiasX;
    float accelBiasY;
    float accelBiasZ;

} Attitude_t;


//==================================================
// PILOT COMMANDS
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
    // Current System Mode
    SystemMode_t mode;

    // Raw Sensor Data
    GY87_State_t sensors;

    // Estimated Attitude
    Attitude_t attitude;

    // Pilot Commands
    Setpoint_t setpoint;

    // Motor Outputs
    MotorMix_t motors;

} FlightState_t;


//==================================================
// GLOBAL STATE
//==================================================

extern FlightState_t g_flightState;


//==================================================
// PUBLIC API
//==================================================

void FlightState_Init(FlightState_t *state);

#endif /* FLIGHTSTATE_H */

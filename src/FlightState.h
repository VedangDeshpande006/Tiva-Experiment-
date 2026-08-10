#ifndef FLIGHTSTATE_H
#define FLIGHTSTATE_H

#include <stdint.h>
#include <stdbool.h>

#include "GY87.h"

/*==================================================
 * ATTITUDE STATE
 *==================================================*/

typedef struct
{
    float roll;
    float pitch;
    float yaw;

    /* Sensor calibration offsets (raw MPU6050 LSB) */
    float gyroBiasX;
    float gyroBiasY;
    float gyroBiasZ;

    float accelBiasX;
    float accelBiasY;
    float accelBiasZ;

    /* Number of calibration samples accumulated */
    uint32_t calibrationSamples;

} Attitude_t;


/*==================================================
 * RC COMMAND SETPOINTS
 *==================================================*/

typedef struct
{
    float targetRoll;
    float targetPitch;
    float targetYawRate;

    /*
     * Throttle command in PWM microseconds.
     * Valid motor-control range:
     * 1000 us = minimum
     * 2000 us = maximum
     */
    uint16_t throttle;

} Setpoint_t;


/*==================================================
 * MOTOR OUTPUTS
 *==================================================*/

typedef struct
{
    /*
     * Motor commands in PWM microseconds.
     * Expected range:
     * 1000 us = minimum
     * 2000 us = maximum
     */
    uint16_t m1;
    uint16_t m2;
    uint16_t m3;
    uint16_t m4;

} MotorMix_t;


/*==================================================
 * FLIGHT MODES
 *==================================================*/

typedef enum
{
    SYSTEM_INIT = 0,
    SYSTEM_CALIBRATING,
    SYSTEM_DISARMED,
    SYSTEM_ARMED,
    SYSTEM_FAILSAFE

} SystemMode_t;


/*==================================================
 * COMPLETE FLIGHT STATE
 *==================================================*/

typedef struct
{
    SystemMode_t mode;

    Attitude_t attitude;

    Setpoint_t setpoint;

    MotorMix_t motors;

} FlightState_t;


/*==================================================
 * PUBLIC FUNCTIONS
 *==================================================*/

/**
 * @brief Initialize flight state variables to safe defaults.
 *
 * Initial mode:
 *     SYSTEM_INIT
 *
 * All attitude, calibration, setpoint and motor values
 * are initialized to safe defaults.
 *
 * If startup calibration is enabled, the flight-state
 * implementation will begin calibration from this state.
 *
 * @param state Pointer to flight state.
 */
void FlightState_Init(FlightState_t *state);


/**
 * @brief Perform one non-blocking calibration step.
 *
 * One IMU sample is accumulated per function call.
 * The function is intended to be called once per
 * control-loop iteration.
 *
 * At 250 Hz, CALIBRATION_SAMPLES samples require
 * approximately CALIBRATION_SAMPLES / 250 seconds.
 *
 * Calibration offsets are stored in the attitude state.
 *
 * @param state Pointer to flight state.
 * @param imu Pointer to latest IMU data.
 *
 * @return true when calibration is complete.
 *         false while calibration is still running.
 */
bool FlightState_UpdateCalibration(FlightState_t *state,
                                    const GY87_IMU_t *imu);


#endif /* FLIGHTSTATE_H */

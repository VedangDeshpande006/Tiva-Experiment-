#include "FlightState.h"
#include "GY87.h"
#include "config.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "driverlib/sysctl.h"

//==================================================
// Delay Macro
//==================================================
#define MS_TO_DELAY(ms) (((SYSTEM_CLOCK_HZ) / 3000UL) * (ms))


//==================================================
// Initialize Flight State
//==================================================
void FlightState_Init(FlightState_t *state)
{
    if (state == NULL)
        return;

    //------------------------------------------------
    // System Mode
    //------------------------------------------------
    state->mode = SYSTEM_INIT;

    //------------------------------------------------
    // Attitude
    //------------------------------------------------
    state->attitude.roll = 0.0f;
    state->attitude.pitch = 0.0f;
    state->attitude.yaw = 0.0f;

    //------------------------------------------------
    // Gyro Bias
    //------------------------------------------------
    state->attitude.gyroBiasX = 0.0f;
    state->attitude.gyroBiasY = 0.0f;
    state->attitude.gyroBiasZ = 0.0f;

    //------------------------------------------------
    // Accelerometer Bias
    //------------------------------------------------
    state->attitude.accelBiasX = 0.0f;
    state->attitude.accelBiasY = 0.0f;
    state->attitude.accelBiasZ = 0.0f;

    //------------------------------------------------
    // RC Setpoints
    //------------------------------------------------
    state->setpoint.targetRoll = 0.0f;
    state->setpoint.targetPitch = 0.0f;
    state->setpoint.targetYawRate = 0.0f;
    state->setpoint.throttle = ESC_MIN_US;

    //------------------------------------------------
    // Motors
    //------------------------------------------------
    state->motors.m1 = ESC_MIN_US;
    state->motors.m2 = ESC_MIN_US;
    state->motors.m3 = ESC_MIN_US;
    state->motors.m4 = ESC_MIN_US;

    //------------------------------------------------
    // Ready
    //------------------------------------------------
    state->mode = SYSTEM_DISARMED;
}


//==================================================
// Gyroscope Calibration
//==================================================
bool FlightState_CalibrateSensors(FlightState_t *state)
{
    if (state == NULL)
        return false;

    state->mode = SYSTEM_CALIBRATING;

    //------------------------------------------------
    // Allow sensors to stabilize
    //------------------------------------------------
    SysCtlDelay(MS_TO_DELAY(500));

    int32_t gyroX_sum = 0;
    int32_t gyroY_sum = 0;
    int32_t gyroZ_sum = 0;

    GY87_State_t sensor;

    //------------------------------------------------
    // Collect Samples
    //------------------------------------------------
    for (uint16_t i = 0; i < GYRO_OFFSET_SAMPLES; i++)
    {
        GY87_UpdateState(&sensor);

        gyroX_sum += sensor.imu.gx;
        gyroY_sum += sensor.imu.gy;
        gyroZ_sum += sensor.imu.gz;

        SysCtlDelay(MS_TO_DELAY(LOOP_PERIOD_MS));
    }

    //------------------------------------------------
    // Compute Average Bias
    //------------------------------------------------
    state->attitude.gyroBiasX =
        (float)gyroX_sum / (float)GYRO_OFFSET_SAMPLES;

    state->attitude.gyroBiasY =
        (float)gyroY_sum / (float)GYRO_OFFSET_SAMPLES;

    state->attitude.gyroBiasZ =
        (float)gyroZ_sum / (float)GYRO_OFFSET_SAMPLES;

    //------------------------------------------------
    // Accelerometer Bias
    //------------------------------------------------
    state->attitude.accelBiasX = 0.0f;
    state->attitude.accelBiasY = 0.0f;
    state->attitude.accelBiasZ = 0.0f;

    //------------------------------------------------
    // Calibration Complete
    //------------------------------------------------
    state->mode = SYSTEM_DISARMED;

    return true;
}

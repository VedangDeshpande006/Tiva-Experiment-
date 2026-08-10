#include "FlightState.h"
#include "config.h"

#include <stdint.h>
#include <stdbool.h>

/*=============================================================================
 * Calibration Accumulators (Static Module State)
 *===========================================================================*/

static int32_t s_gyroX_sum = 0;
static int32_t s_gyroY_sum = 0;
static int32_t s_gyroZ_sum = 0;

static int32_t s_accelX_sum = 0;
static int32_t s_accelY_sum = 0;
static int32_t s_accelZ_sum = 0;

static uint16_t s_calibration_count = 0;
static uint16_t s_discard_count = 0;

/*=============================================================================
 * FlightState_Init
 *===========================================================================*/

void FlightState_Init(FlightState_t *state)
{
    if (state == NULL)
    {
        return;
    }

    state->mode = SYSTEM_INIT;

    state->attitude.roll = 0.0f;
    state->attitude.pitch = 0.0f;
    state->attitude.yaw = 0.0f;

    state->attitude.gyroBiasX = 0.0f;
    state->attitude.gyroBiasY = 0.0f;
    state->attitude.gyroBiasZ = 0.0f;

    state->attitude.accelBiasX = 0.0f;
    state->attitude.accelBiasY = 0.0f;
    state->attitude.accelBiasZ = 0.0f;

    state->attitude.calibrationSamplesCollected = 0U;

    state->setpoint.targetRoll = 0.0f;
    state->setpoint.targetPitch = 0.0f;
    state->setpoint.targetYawRate = 0.0f;
    state->setpoint.throttle = ESC_MIN_US;

    state->motors.m1 = ESC_MIN_US;
    state->motors.m2 = ESC_MIN_US;
    state->motors.m3 = ESC_MIN_US;
    state->motors.m4 = ESC_MIN_US;

    s_gyroX_sum = 0;
    s_gyroY_sum = 0;
    s_gyroZ_sum = 0;

    s_accelX_sum = 0;
    s_accelY_sum = 0;
    s_accelZ_sum = 0;

    s_calibration_count = 0U;
    s_discard_count = 0U;
}

/*=============================================================================
 * FlightState_UpdateCalibration
 *
 * Non-blocking IMU calibration. Called once per control-loop iteration
 * (250 Hz) while system is in SYSTEM_CALIBRATING mode.
 *
 * Sequence:
 *   - Discard first 125 samples (~500 ms for sensor stabilization)
 *   - Accumulate next 1000 samples (~4 seconds)
 *   - Compute gyro and accel biases by averaging
 *   - Return true when complete; caller transitions to SYSTEM_DISARMED
 *
 * Accel Z bias: subtracts 1g reference (4096 LSB at ±8g scale)
 * because during level calibration, Z-axis should read ~4096 (1g).
 *===========================================================================*/

bool FlightState_UpdateCalibration(FlightState_t *state,
                                   const GY87_IMU_t *imu)
{
    if ((state == NULL) || (imu == NULL))
    {
        return false;
    }

    if (state->mode != SYSTEM_CALIBRATING)
    {
        return false;
    }

    if (s_discard_count < (CONTROL_LOOP_HZ / 2U))
    {
        s_discard_count++;
        return false;
    }

    s_gyroX_sum += (int32_t)imu->gx;
    s_gyroY_sum += (int32_t)imu->gy;
    s_gyroZ_sum += (int32_t)imu->gz;

    s_accelX_sum += (int32_t)imu->ax;
    s_accelY_sum += (int32_t)imu->ay;
    s_accelZ_sum += (int32_t)imu->az;

    s_calibration_count++;
    state->attitude.calibrationSamplesCollected = (uint32_t)s_calibration_count;

    if (s_calibration_count >= CALIBRATION_SAMPLES)
    {
        state->attitude.gyroBiasX =
            (float)s_gyroX_sum / (float)CALIBRATION_SAMPLES;

        state->attitude.gyroBiasY =
            (float)s_gyroY_sum / (float)CALIBRATION_SAMPLES;

        state->attitude.gyroBiasZ =
            (float)s_gyroZ_sum / (float)CALIBRATION_SAMPLES;

        state->attitude.accelBiasX =
            (float)s_accelX_sum / (float)CALIBRATION_SAMPLES;

        state->attitude.accelBiasY =
            (float)s_accelY_sum / (float)CALIBRATION_SAMPLES;

        state->attitude.accelBiasZ =
            ((float)s_accelZ_sum / (float)CALIBRATION_SAMPLES)
            - (1.0f / ACCEL_SCALE);

        s_gyroX_sum = 0;
        s_gyroY_sum = 0;
        s_gyroZ_sum = 0;

        s_accelX_sum = 0;
        s_accelY_sum = 0;
        s_accelZ_sum = 0;

        s_calibration_count = 0U;
        s_discard_count = 0U;

        state->attitude.calibrationSamplesCollected = 0U;

        state->mode = SYSTEM_DISARMED;

        return true;
    }

    return false;
}

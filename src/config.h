#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 * 1. ESC & MOTOR CONFIGURATION
 * Flashed SimonK ESCs -> Disable OneShot125 to run high-speed standard PWM.
 * Layout follows X-configuration:
 * Motor 1: Bottom Right (CW) | Motor 2: Top Right (CCW)
 * Motor 3: Bottom Left  (CCW)| Motor 4: Top Left  (CW)
 *****************************************************************************/
#define ONESHOT125          0       // Standard SimonK uses normal high-speed PWM
#define PWM_FREQUENCY       400     // 400Hz update rate

// Hardware PWM Pin Mappings (PWM0 Module, Generator 0 & 1)
// Used by Motors.c
// Motor 1: PB6 (M0PWM0)
// Motor 2: PB7 (M0PWM1)
// Motor 3: PB4 (M0PWM2)
// Motor 4: PB5 (M0PWM3)

/******************************************************************************
 * 2. SENSOR & I2C SETUP (GY-87 Module)
 * Uses Tiva I2C0: PB2 (SCL) and PB3 (SDA).
 *****************************************************************************/
#define MPU6050_I2C_ADDRESS 0x68
#define HMC5883L_I2C_ADDRESS 0x1E
#define BMP180_I2C_ADDRESS  0x77
#define MPU6050_DLPF_CFG    4       // 20Hz bandwidth to reject frame vibrations

/******************************************************************************
 * 3. RC RECEIVER THRESHOLDS (PPM/CPPM Input)
 *****************************************************************************/
#define THROTTLE_MIN        1000    // Minimum pulse length (Idle)
#define THROTTLE_MAX        2000    // Maximum pulse length (Full Power)
#define MIN_CHECK           1100    // Rudder stick threshold to trigger arming/disarming
#define MAX_CHECK           1900

/******************************************************************************
 * 4. DEFAULT PID VALUES (Starting Points for F450 Frame)
 *****************************************************************************/
#define PID_ROLL_P          1.0f
#define PID_ROLL_I          0.0f
#define PID_ROLL_D          0.0f

#define PID_PITCH_P         1.0f
#define PID_PITCH_I         0.0f
#define PID_PITCH_D         0.0f

#define PID_YAW_P           2.0f
#define PID_YAW_I           0.0f
#define PID_YAW_D           0.0f

#endif /* _CONFIG_H_ */

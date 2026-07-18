#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 * 1. ESC & MOTOR CONFIGURATION
 * Flashed SimonK ESCs -> Disable OneShot125 to run high-speed standard PWM.
 * Layout follows Naze32 X-configuration:
 * Motor 1: Bottom Right (CW) | Motor 2: Top Right (CCW)
 * Motor 3: Bottom Left  (CCW)| Motor 4: Top Left  (CW)
 *****************************************************************************/
#define ONESHOT125          0       // Set to 0 because standard SimonK uses normal high-speed PWM
#define PWM_FREQUENCY       400     // 400Hz update rate standard for SimonK ESCs

// Hardware PWM Pin Mappings (Tiva C Hardware Blocks)
#define MOTOR1_BASE         GPIO_PORTB_BASE
#define MOTOR1_PIN          GPIO_PIN_6          // M0PWM0
#define MOTOR2_BASE         GPIO_PORTB_BASE
#define MOTOR2_PIN          GPIO_PIN_7          // M0PWM1
#define MOTOR3_BASE         GPIO_PORTB_BASE
#define MOTOR3_PIN          GPIO_PIN_4          // M0PWM2
#define MOTOR4_BASE         GPIO_PORTB_BASE
#define MOTOR4_PIN          GPIO_PIN_5          // M0PWM3

/******************************************************************************
 * 2. SENSOR & I2C SETUP (GY-87 Module)
 * GY-87 includes MPU6050 (Acc/Gyro) at address 0x68.
 * Uses Tiva I2C0: PB2 (SCL) and PB3 (SDA).
 *****************************************************************************/
#define MPU6050_I2C_ADDRESS 0x68
#define MPU6050_DLPF_CFG    4       // Digital Low Pass Filter: 20Hz bandwidth to reject frame vibrations

/******************************************************************************
 * 3. RC RECEIVER THRESHOLDS (PPM/CPPM Input)
 * Standard RC timing limits measured in microseconds.
 *****************************************************************************/
#define THROTTLE_MIN        1000    // Minimum pulse length (Idle)
#define THROTTLE_MAX        2000    // Maximum pulse length (Full Power)
#define MIN_CHECK           1100    // Rudder stick threshold to trigger arming/disarming
#define MAX_CHECK           1900

/******************************************************************************
 * 4. DEFAULT PID VALUES (Starting Points for F450 Frame)
 * These values will look for errors and handle corrections.
 *****************************************************************************/
// Roll & Pitch Rate PID (Inner Loop)
#define PID_ROLL_P          1.0f
#define PID_ROLL_I          0.0f
#define PID_ROLL_D          0.0f

#define PID_PITCH_P         1.0f
#define PID_PITCH_I         0.0f
#define PID_PITCH_D         0.0f

// Yaw Rate PID
#define PID_YAW_P           2.0f
#define PID_YAW_I          0.0f
#define PID_YAW_D           0.0f

#endif /* _CONFIG_H_ */      

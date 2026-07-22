#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 * 1. ESC & MOTOR CONFIGURATION
 * Flashed SimonK ESCs -> Disable OneShot125 to run high-speed standard PWM.[cite: 2]
 * Layout follows X-configuration:[cite: 2]
 * Motor 1: Bottom Right (CW) | Motor 2: Top Right (CCW)[cite: 2]
 * Motor 3: Bottom Left  (CCW)| Motor 4: Top Left  (CW)[cite: 2]
 *****************************************************************************/
#define ONESHOT125          0       // Standard SimonK uses normal high-speed PWM[cite: 2]
#define PWM_FREQUENCY       400     // 400Hz update rate[cite: 2]

// Hardware PWM Pin Mappings (Timer 1 and Timer 2)[cite: 2]
#define MOTOR1_BASE         GPIO_PORTB_BASE[cite: 2]
#define MOTOR1_PIN          GPIO_PIN_6          // T1CCP0[cite: 2]
#define MOTOR2_BASE         GPIO_PORTB_BASE[cite: 2]
#define MOTOR2_PIN          GPIO_PIN_7          // T1CCP1[cite: 2]
#define MOTOR3_BASE         GPIO_PORTB_BASE[cite: 2]
#define MOTOR3_PIN          GPIO_PIN_4          // T2CCP0[cite: 2]
#define MOTOR4_BASE         GPIO_PORTB_BASE[cite: 2]
#define MOTOR4_PIN          GPIO_PIN_5          // T2CCP1[cite: 2]

/******************************************************************************
 * 2. SENSOR & I2C SETUP (GY-87 Module)
 * Uses Tiva I2C0: PB2 (SCL) and PB3 (SDA).[cite: 2]
 *****************************************************************************/
#define MPU6050_I2C_ADDRESS 0x68[cite: 2]
#define HMC5883L_I2C_ADDRESS 0x1E
#define BMP180_I2C_ADDRESS  0x77
#define MPU6050_DLPF_CFG    4       // 20Hz bandwidth to reject frame vibrations[cite: 2]

/******************************************************************************
 * 3. RC RECEIVER THRESHOLDS (PPM/CPPM Input)[cite: 2]
 *****************************************************************************/
#define THROTTLE_MIN        1000    // Minimum pulse length (Idle)[cite: 2]
#define THROTTLE_MAX        2000    // Maximum pulse length (Full Power)[cite: 2]
#define MIN_CHECK           1100    // Rudder stick threshold to trigger arming/disarming[cite: 2]
#define MAX_CHECK           1900[cite: 2]

/******************************************************************************
 * 4. DEFAULT PID VALUES (Starting Points for F450 Frame)[cite: 2]
 *****************************************************************************/
#define PID_ROLL_P          1.0f[cite: 2]
#define PID_ROLL_I          0.0f[cite: 2]
#define PID_ROLL_D          0.0f[cite: 2]

#define PID_PITCH_P         1.0f[cite: 2]
#define PID_PITCH_I         0.0f[cite: 2]
#define PID_PITCH_D         0.0f[cite: 2]

#define PID_YAW_P           2.0f[cite: 2]
#define PID_YAW_I           0.0f[cite: 2]
#define PID_YAW_D           0.0f[cite: 2]

#endif /* _CONFIG_H_ */

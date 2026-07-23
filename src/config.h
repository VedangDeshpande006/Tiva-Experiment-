#ifndef CONFIG_H
#define CONFIG_H

/******************************************************************************
 * Project:
 *      Autonomous Quadcopter Flight Controller
 *
 * MCU:
 *      Texas Instruments TM4C123GH6PM
 *      ARM Cortex-M4F @ 80 MHz
 *
 * Description:
 *      Global firmware configuration.
 *      This file is the ONLY location where hardware configuration,
 *      control parameters and project-wide constants should be defined.
 ******************************************************************************/

#include <stdint.h>

/*=============================================================================
 * SYSTEM CLOCK
 *===========================================================================*/

#define SYSTEM_CLOCK_HZ                80000000UL

/*=============================================================================
 * CONTROL LOOP
 *===========================================================================*/

#define CONTROL_LOOP_HZ                250U
#define CONTROL_LOOP_DT_SEC            (1.0f / CONTROL_LOOP_HZ)
#define LOOP_PERIOD_MS                 (1000U / CONTROL_LOOP_HZ)

/*=============================================================================
 * SYSTEM TICK
 *===========================================================================*/

#define SYSTICK_HZ                     1000U

/*=============================================================================
 * SENSOR STARTUP
 *===========================================================================*/

#define SENSOR_STARTUP_DELAY_MS        100U

/*=============================================================================
 * SENSOR I2C ADDRESSES
 *===========================================================================*/

#define MPU6050_I2C_ADDRESS            0x68
#define HMC5883L_I2C_ADDRESS           0x1E
#define BMP180_I2C_ADDRESS             0x77

/*=============================================================================
 * MPU6050 CONFIGURATION
 *===========================================================================*/

/* Digital Low Pass Filter (~42Hz) */
#define MPU6050_DLPF_CFG               0x03

/* Gyroscope ±2000 deg/sec */
#define MPU6050_GYRO_FS_2000           0x18

/* Accelerometer ±8g */
#define MPU6050_ACCEL_FS_8             0x10

/* Sample Rate Divider
 *
 * Sample Rate = 1000 / (1 + Divider)
 * Divider = 3
 * Output = 250 Hz
 */
#define MPU6050_SMPLRT_DIV_VALUE       0x03

/*=============================================================================
 * SENSOR SCALING
 *===========================================================================*/

#define ACCEL_SCALE                    (1.0f / 4096.0f)
#define GYRO_SCALE                     (1.0f / 16.4f)

/*=============================================================================
 * STARTUP CALIBRATION
 *===========================================================================*/

/*
 * 1 = Perform automatic calibration during startup.
 * 0 = Use fixed offsets defined below.
 */
#define USE_STARTUP_CALIBRATION        1U

#define CALIBRATION_SAMPLES            1000U
#define CALIBRATION_PERIOD_MS          LOOP_PERIOD_MS

#if (USE_STARTUP_CALIBRATION == 0)

#define GYRO_X_OFFSET                  0.0f
#define GYRO_Y_OFFSET                  0.0f
#define GYRO_Z_OFFSET                  0.0f

#define ACCEL_X_OFFSET                 0.0f
#define ACCEL_Y_OFFSET                 0.0f
#define ACCEL_Z_OFFSET                 0.0f

#endif

/*=============================================================================
 * FILTER PARAMETERS
 *===========================================================================*/

#define KALMAN_Q_ANGLE                 0.001f
#define KALMAN_Q_BIAS                  0.003f
#define KALMAN_R_MEASURE               0.03f

#define COMPLEMENTARY_ALPHA            0.98f

/*=============================================================================
 * FLIGHT LIMITS
 *===========================================================================*/

#define MAX_ROLL_ANGLE                 45.0f
#define MAX_PITCH_ANGLE                45.0f
#define MAX_YAW_RATE                   180.0f

/*=============================================================================
 * PID DEFAULT GAINS
 *
 * Typical tuning ranges:
 *
 * KP : 2.0 - 6.0
 * KI : 0.0 - 0.2
 * KD : 0.02 - 0.20
 *===========================================================================*/

#define PID_ROLL_KP                    3.5f
#define PID_ROLL_KI                    0.0f
#define PID_ROLL_KD                    0.08f

#define PID_PITCH_KP                   3.5f
#define PID_PITCH_KI                   0.0f
#define PID_PITCH_KD                   0.08f

#define PID_YAW_KP                     2.5f
#define PID_YAW_KI                     0.0f
#define PID_YAW_KD                     0.0f

/*=============================================================================
 * ESC CONFIGURATION
 *===========================================================================*/

#define ESC_PWM_HZ                     400U

#define ESC_MIN_US                     1000U
#define ESC_MID_US                     1500U
#define ESC_MAX_US                     2000U

#define MOTOR_ARM_DELAY_MS             2000U

/*=============================================================================
 * PWM HARDWARE
 *===========================================================================*/

/*
 * PWM Clock
 *
 * 80 MHz / 64 = 1.25 MHz
 */

#define PWM_CLOCK_DIVIDER              64U

#define PWM_CLOCK_HZ                   (SYSTEM_CLOCK_HZ / PWM_CLOCK_DIVIDER)

#define PWM_PERIOD_TICKS               (PWM_CLOCK_HZ / ESC_PWM_HZ)

/* Integer conversion used by Motors.c */
#define PWM_TICKS(us)                  (((us) * PWM_CLOCK_HZ) / 1000000U)

/*=============================================================================
 * MOTOR NUMBERING (LOCKED)
 *
 *                 FRONT
 *
 *            M1(CCW)    M2(CW)
 *
 *
 *            M4(CW)     M3(CCW)
 *
 *                 REAR
 *===========================================================================*/

#define MOTOR_COUNT                    4U

/*=============================================================================
 * TM4C PIN ASSIGNMENTS (LOCKED)
 *===========================================================================*/

/* ---------------- UART1 ---------------- */

#define RC_UART_BAUD                   115200U

#define RC_UART_RX_PIN                 0U
#define RC_UART_TX_PIN                 1U

/* ---------------- I2C0 ---------------- */

#define I2C_SCL_PIN                    2U
#define I2C_SDA_PIN                    3U

/* ---------------- PWM ---------------- */

#define MOTOR1_PWM_PIN                 6U
#define MOTOR2_PWM_PIN                 7U
#define MOTOR3_PWM_PIN                 4U
#define MOTOR4_PWM_PIN                 5U

/*=============================================================================
 * FAILSAFE TIMING
 *
 * RC_TIMEOUT
 *      Ignore stale packets.
 *
 * FAILSAFE_TIMEOUT
 *      Force motors to idle.
 *
 * AUTO_DISARM_TIMEOUT
 *      Fully disarm aircraft.
 *===========================================================================*/

#define RC_TIMEOUT_MS                  250U
#define FAILSAFE_TIMEOUT_MS            500U
#define AUTO_DISARM_TIMEOUT_MS         3000U

/*=============================================================================
 * MAGNETOMETER
 *===========================================================================*/

/*
 * Local magnetic declination.
 *
 * Update for deployment location.
 * This value can later be replaced automatically
 * using GPS-based location.
 */
#define MAG_DECLINATION_DEG            0.0f

#endif /* CONFIG_H */

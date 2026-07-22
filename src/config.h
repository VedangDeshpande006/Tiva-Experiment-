#ifndef CONFIG_H
#define CONFIG_H

//==================================================
// 1. SYSTEM TIMING & CONTROL LOOP
//==================================================
#define SYSTEM_CLOCK_HZ             80000000UL      // 80 MHz
#define CONTROL_LOOP_HZ             250U            // Flight loop frequency
#define CONTROL_LOOP_DT_SEC         (1.0f / CONTROL_LOOP_HZ)
#define LOOP_PERIOD_MS              (1000U / CONTROL_LOOP_HZ)
#define SENSOR_STARTUP_DELAY_MS     100U

//==================================================
// 2. SENSOR I2C ADDRESSES
//==================================================
#define MPU6050_I2C_ADDRESS         0x68
#define HMC5883L_I2C_ADDRESS        0x1E

//==================================================
// 3. MPU6050 CONFIGURATION
//==================================================

// Digital Low Pass Filter (~42Hz)
#define MPU6050_DLPF_CFG            0x03

// Gyroscope Full Scale = ±2000 deg/sec
#define MPU6050_GYRO_FS_2000        0x18

// Accelerometer Full Scale = ±8g
#define MPU6050_ACCEL_FS_8          0x10

// Sample Rate Divider
// Sample Rate = 1000Hz / (1 + Divider)
// Divider = 3 --> 250Hz
#define MPU6050_SMPLRT_DIV_VALUE    0x03

//==================================================
// 4. SENSOR SCALING FACTORS
//==================================================

// Accelerometer (±8g)
#define ACCEL_SCALE                 (1.0f / 4096.0f)

// Gyroscope (±2000 dps)
#define GYRO_SCALE                  (1.0f / 16.4f)

//==================================================
// 5. SENSOR CALIBRATION
//==================================================

// Number of samples used during startup calibration
#define GYRO_OFFSET_SAMPLES         1000U
#define ACCEL_OFFSET_SAMPLES        1000U

// Factory offsets (updated after calibration)
#define GYRO_X_OFFSET               0.0f
#define GYRO_Y_OFFSET               0.0f
#define GYRO_Z_OFFSET               0.0f

#define ACCEL_X_OFFSET              0.0f
#define ACCEL_Y_OFFSET              0.0f
#define ACCEL_Z_OFFSET              0.0f

//==================================================
// 6. FILTER PARAMETERS
//==================================================

// Kalman Filter
#define KALMAN_Q_ANGLE             0.001f
#define KALMAN_Q_BIAS              0.003f
#define KALMAN_R_MEASURE           0.03f

// Complementary Filter
#define COMPLEMENTARY_ALPHA        0.98f

//==================================================
// 7. FLIGHT LIMITS
//==================================================
#define MAX_ROLL_ANGLE             45.0f
#define MAX_PITCH_ANGLE            45.0f
#define MAX_YAW_RATE               180.0f

//==================================================
// 8. PID DEFAULT GAINS
//==================================================

// Roll
#define PID_ROLL_KP                3.5f
#define PID_ROLL_KI                0.0f
#define PID_ROLL_KD                0.08f

// Pitch
#define PID_PITCH_KP               3.5f
#define PID_PITCH_KI               0.0f
#define PID_PITCH_KD               0.08f

// Yaw
#define PID_YAW_KP                 2.5f
#define PID_YAW_KI                 0.0f
#define PID_YAW_KD                 0.00f

//==================================================
// 9. ESC / MOTOR SETTINGS
//==================================================

// ESC PWM Frequency
#define ESC_PWM_HZ                 400U

// PWM Period in microseconds
#define ESC_PWM_PERIOD_US          (1000000U / ESC_PWM_HZ)

// ESC Pulse Width Limits
#define ESC_MIN_US                 1000U
#define ESC_MAX_US                 2000U

// ESC Arming Delay
#define MOTOR_ARM_DELAY_MS         2000U

//==================================================
// 10. PWM HARDWARE SETTINGS
//==================================================

// TM4C123 PWM Clock = 80MHz / 64 = 1.25MHz
#define PWM_CLOCK_DIVIDER          64U

// PWM Clock Frequency
#define PWM_CLOCK_HZ               (SYSTEM_CLOCK_HZ / PWM_CLOCK_DIVIDER)

// PWM Period Ticks
#define PWM_PERIOD_TICKS           (PWM_CLOCK_HZ / ESC_PWM_HZ)

// Tick Conversion
#define PWM_TICKS_PER_US           ((float)PWM_CLOCK_HZ / 1000000.0f)

//==================================================
// 11. RC COMMUNICATION
//==================================================
#define RC_BAUD_RATE               115200U

// Receiver timeout before failsafe
#define RC_TIMEOUT_MS              250U

#endif /* CONFIG_H */

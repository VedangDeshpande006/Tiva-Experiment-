#include <math.h>
#include <stdbool.h>
#include "GY87.h"
#include "config.h"
#include "I2C_Utils.h"
#include "driverlib/sysctl.h" 

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// Constants for Temperature conversion
#define MPU6050_TEMP_SENSITIVITY 340.0f
#define MPU6050_TEMP_OFFSET      36.53f

// Internal function to break the MPU6050 master lock and bridge the I2C lines
static void GY87_EnableBypassMode(void) {
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_USER_CTRL, 0x00);
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_INT_PIN_CFG, 0x02);
}

bool GY87_Init(void) {
    uint8_t id_buffer = 0;
    
    // 1. Verify MPU6050 Connection
    I2C_ReadBurst(MPU6050_I2C_ADDRESS, MPU6050_WHO_AM_I, &id_buffer, 1);
    if (id_buffer != MPU6050_WHO_AM_I_VALUE) {
        return false; // MPU6050 missing or I2C bus error
    }

    // 2. Wake up MPU6050
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_PWR_MGMT_1, 0x00);
    
    // 3. Stabilization Delay (~100ms) to ensure PLL is ready
    SysCtlDelay(SysCtlClockGet() / 30); 
    
    // 4. Configure Rates, Scales, and Filters
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_SMPLRT_DIV, MPU6050_SMPLRT_250HZ);
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_GYRO_CONFIG, MPU6050_GYRO_FS_2000);
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_ACCEL_CONFIG, MPU6050_ACCEL_FS_8);
    I2C_WriteByte(MPU6050_I2C_ADDRESS, MPU6050_CONFIG, MPU6050_DLPF_CFG);
    
    // 5. Enable Magnetometer/Barometer Bypass
    GY87_EnableBypassMode();
    
    // 6. Verify HMC5883L Connection (Requires Bypass to be active)
    I2C_ReadBurst(HMC5883L_I2C_ADDRESS, HMC5883L_REG_IDA, &id_buffer, 1);
    if (id_buffer != HMC5883L_IDA_VALUE) {
        return false; // HMC5883L missing, bypass failed, or I2C error
    }
    
    // 7. Initialize HMC5883L (8-sample average, 15Hz output, continuous mode)
    I2C_WriteByte(HMC5883L_I2C_ADDRESS, HMC5883L_REG_CONFIG_A, 0x70);
    I2C_WriteByte(HMC5883L_I2C_ADDRESS, HMC5883L_REG_MODE, 0x00);
    
    return true; // Entire GY-87 module initialized successfully
}

void GY87_UpdateState(GY87_State_t* state) {
    uint8_t imuBuffer[14];
    uint8_t magBuffer[6];
    
    // ========================================================================
    // 1. READ MPU6050
    // ========================================================================
    I2C_ReadBurst(MPU6050_I2C_ADDRESS, MPU6050_ACCEL_XOUT_H, imuBuffer, 14);
    
    state->imu.ax = (int16_t)((imuBuffer[0] << 8)  | imuBuffer[1]);
    state->imu.ay = (int16_t)((imuBuffer[2] << 8)  | imuBuffer[3]);
    state->imu.az = (int16_t)((imuBuffer[4] << 8)  | imuBuffer[5]);
    
    int16_t rawTemp = (int16_t)((imuBuffer[6] << 8) | imuBuffer[7]);
    state->imu.temperature = ((float)rawTemp / MPU6050_TEMP_SENSITIVITY) + MPU6050_TEMP_OFFSET;
    
    state->imu.gx = (int16_t)((imuBuffer[8] << 8)  | imuBuffer[9]);
    state->imu.gy = (int16_t)((imuBuffer[10] << 8) | imuBuffer[11]);
    state->imu.gz = (int16_t)((imuBuffer[12] << 8) | imuBuffer[13]);
    
    // ========================================================================
    // 2. READ HMC5883L
    // ========================================================================
    I2C_ReadBurst(HMC5883L_I2C_ADDRESS, HMC5883L_REG_DATA_START, magBuffer, 6);
    
    // HMC5883L registers store data in a distinct sequence: X, Z, Y
    state->magnetometer.mx = (int16_t)((magBuffer[0] << 8) | magBuffer[1]);
    state->magnetometer.mz = (int16_t)((magBuffer[2] << 8) | magBuffer[3]);
    state->magnetometer.my = (int16_t)((magBuffer[4] << 8) | magBuffer[5]);
    
    // ========================================================================
    // 3. COMPUTE & CLAMP HEADING
    // ========================================================================
    float heading = atan2f((float)state->magnetometer.my, (float)state->magnetometer.mx);
    if (heading < 0.0f) {
        heading += 2.0f * M_PI;
    }
    state->magnetometer.heading = heading;
}

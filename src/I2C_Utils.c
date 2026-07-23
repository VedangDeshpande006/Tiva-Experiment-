#ifndef I2C_UTILS_H
#define I2C_UTILS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//==================================================
// I2C0 Hardware Configuration
//==================================================
//
// Controller : I2C0
// SCL        : PB2
// SDA        : PB3
// Bus Speed  : 100 kHz (Standard Mode)
//
// All slave addresses passed to these functions are
// 7-bit addresses exactly as specified in device
// datasheets.
//
// Example:
// MPU6050  -> 0x68
// HMC5883L -> 0x1E
// BMP180   -> 0x77
//
//==================================================


//==================================================
// Driver Configuration
//==================================================

// Maximum polling iterations while waiting for an
// I2C transfer to complete.
//
// This prevents software from hanging forever if a
// sensor is disconnected or the bus becomes stuck.
//
// Increase this value if running at slower bus
// speeds.
#define I2C_TIMEOUT_COUNT      100000UL


//==================================================
// Driver Status Codes
//==================================================

typedef enum
{
    I2C_STATUS_OK = 0,

    I2C_STATUS_TIMEOUT,

    I2C_STATUS_BUS_ERROR,

    I2C_STATUS_INVALID_PARAMETER

} I2C_Status_t;


//==================================================
// Global Driver Status
//==================================================

// Contains the result of the most recent I2C
// transaction.
//
// This is intended for debugging and future UART
// telemetry logging.
//
extern volatile I2C_Status_t g_i2cLastStatus;


//==================================================
// Public API
//==================================================

/**
 * @brief Initialize I2C0 peripheral.
 *
 * Configures:
 *      PB2 -> SCL
 *      PB3 -> SDA
 *
 * Standard Mode (100 kHz)
 */
void I2C0_Init(void);


/**
 * @brief Write one byte to a slave register.
 *
 * @param slaveAddr
 *      7-bit I2C slave address.
 *
 * @param regAddr
 *      Register address.
 *
 * @param data
 *      Data byte.
 *
 * @return true on success.
 * @return false on timeout or bus error.
 */
bool I2C_WriteByte(uint8_t slaveAddr,
                   uint8_t regAddr,
                   uint8_t data);


/**
 * @brief Read consecutive bytes from a slave.
 *
 * Uses repeated-start transaction.
 *
 * @param slaveAddr
 *      7-bit slave address.
 *
 * @param startRegAddr
 *      First register.
 *
 * @param buffer
 *      Destination buffer.
 *
 * @param length
 *      Number of bytes.
 *
 *      length == 0 returns false.
 *
 * @return true on success.
 * @return false on timeout or bus error.
 */
bool I2C_ReadBurst(uint8_t slaveAddr,
                   uint8_t startRegAddr,
                   uint8_t *buffer,
                   uint32_t length);


/**
 * @brief Returns last driver status.
 */
I2C_Status_t I2C_GetLastStatus(void);


#ifdef __cplusplus
}
#endif

#endif /* I2C_UTILS_H */

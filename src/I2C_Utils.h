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
// All slave addresses passed to this driver are
// 7-bit addresses exactly as specified in sensor
// datasheets.
//
// Current Sensors:
//     MPU6050  -> 0x68
//     HMC5883L -> 0x1E
//     BMP180   -> 0x77
//
//==================================================


//==================================================
// Driver Configuration
//==================================================

// Maximum polling iterations while waiting for an
// I2C transfer to complete.
//
// Prevents the firmware from blocking forever if an
// I2C slave is disconnected or the bus becomes stuck.
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
// transaction. Intended for debugging and future
// telemetry logging.
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
 * Bus Speed:
 *      100 kHz Standard Mode
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
 * @brief Read multiple consecutive registers.
 *
 * Uses a repeated-start transaction.
 *
 * @param slaveAddr
 *      7-bit slave address.
 *
 * @param startRegAddr
 *      Starting register.
 *
 * @param buffer
 *      Destination buffer.
 *
 * @param length
 *      Number of bytes to read.
 *
 *      Passing length == 0 returns false.
 *
 * @return true on success.
 * @return false on timeout or bus error.
 */
bool I2C_ReadBurst(uint8_t slaveAddr,
                   uint8_t startRegAddr,
                   uint8_t *buffer,
                   uint32_t length);


/**
 * @brief Returns the status of the most recent
 *        I2C transaction.
 */
I2C_Status_t I2C_GetLastStatus(void);


//==================================================
// Future Driver Expansion
//==================================================
//
// A multi-byte write routine (I2C_WriteBurst) is
// intentionally NOT implemented in Phase 1 because
// none of the current onboard devices require burst
// write operations.
//
// Current devices (MPU6050, HMC5883L and BMP180)
// are fully configured using single-register writes.
//
// If future hardware (EEPROM, OLED display,
// FRAM, etc.) requires consecutive register writes,
// implement:
//
// bool I2C_WriteBurst(uint8_t slaveAddr,
//                     uint8_t startRegAddr,
//                     const uint8_t *buffer,
//                     uint32_t length);
//
// together with timeout and error handling consistent
// with the existing driver.
//
//==================================================

#ifdef __cplusplus
}
#endif

#endif /* I2C_UTILS_H */

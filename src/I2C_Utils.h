#ifndef _I2C_UTILS_H_
#define _I2C_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

// ======================================================
// I2C Driver API
// ======================================================

// Initialize I2C0 peripheral (PB2=SCL, PB3=SDA)
void I2C0_Init(void);

// Write one byte to a register
// Returns true on success, false on I2C error
bool I2C_WriteByte(uint8_t slaveAddr,
                   uint8_t regAddr,
                   uint8_t data);

// Read multiple consecutive bytes
// Returns true on success, false on I2C error
bool I2C_ReadBurst(uint8_t slaveAddr,
                   uint8_t startRegAddr,
                   uint8_t *buffer,
                   uint32_t length);

#endif /* _I2C_UTILS_H_ */

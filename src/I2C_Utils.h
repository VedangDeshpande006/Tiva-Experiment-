#ifndef _I2C_UTILS_H_
#define _I2C_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

// Initialize I2C0 peripheral
void I2C0_Init(void);

// Write one byte
bool I2C_WriteByte(uint8_t slaveAddr,
                   uint8_t regAddr,
                   uint8_t data);

// Read consecutive bytes
bool I2C_ReadBurst(uint8_t slaveAddr,
                   uint8_t startRegAddr,
                   uint8_t *buffer,
                   uint32_t length);

#endif

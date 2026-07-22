#ifndef _I2C_UTILS_H_
#define _I2C_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

// Initialize the I2C0 peripheral on PB2 and PB3
void I2C0_Init(void);

// Write a single byte to any I2C device register
void I2C_WriteByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t data);

// Read multiple consecutive bytes from any I2C device
void I2C_ReadBurst(uint8_t slaveAddr, uint8_t startRegAddr, uint8_t *buffer, uint32_t length);

#endif /* _I2C_UTILS_H_ */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"
#include "MPU6050.h"

// Write a single byte to an MPU6050 register
void I2C_WriteByte(uint8_t regAddr, uint8_t data) {
    // Set slave address for writing
    I2CMasterSlaveAddrSet(I2C0_BASE, MPU6050_ADDRESS, false);
    
    // Transmit register address
    I2CMasterDataPut(I2C0_BASE, regAddr);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));
    
    // Transmit data byte
    I2CMasterDataPut(I2C0_BASE, data);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C0_BASE));
}

// Read multiple consecutive bytes from the sensor
void I2C_ReadBurst(uint8_t startRegAddr, uint8_t *buffer, uint32_t length) {
    // 1. Tell the MPU6050 which register we want to start reading from
    I2CMasterSlaveAddrSet(I2C0_BASE, MPU6050_ADDRESS, false);
    I2CMasterDataPut(I2C0_BASE, startRegAddr);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy(I2C0_BASE));

    // 2. Switch to read mode
    I2CMasterSlaveAddrSet(I2C0_BASE, MPU6050_ADDRESS, true);

    if (length == 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        while(I2CMasterBusy(I2C0_BASE));
        buffer[0] = I2CMasterDataGet(I2C0_BASE);
        return;
    }

    // Receive first byte
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(I2CMasterBusy(I2C0_BASE));
    buffer[0] = I2CMasterDataGet(I2C0_BASE);

    // Receive intermediate bytes
    for (uint32_t i = 1; i < length - 1; i++) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        while(I2CMasterBusy(I2C0_BASE));
        buffer[i] = I2CMasterDataGet(I2C0_BASE);
    }

    // Receive final byte
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(I2CMasterBusy(I2C0_BASE));
    buffer[length - 1] = I2CMasterDataGet(I2C0_BASE);
}

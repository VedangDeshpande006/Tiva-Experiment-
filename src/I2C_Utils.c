#include "I2C_Utils.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"

void I2C0_Init(void) {
    // 1. Enable I2C0 and GPIOB peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    // Wait for peripherals to become ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    // 2. Configure PB2 for I2C0SCL and PB3 for I2C0SDA
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // 3. Initialize Master (Standard 100kHz mode, false for normal speed)
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
}

// Write a single byte to any I2C device register
void I2C_WriteByte(uint8_t slaveAddr, uint8_t regAddr, uint8_t data) {
    I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddr, false);
    I2CMasterDataPut(I2C0_BASE, regAddr);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));
    
    I2CMasterDataPut(I2C0_BASE, data);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C0_BASE));
}

// Read multiple consecutive bytes from any I2C device
void I2C_ReadBurst(uint8_t slaveAddr, uint8_t startRegAddr, uint8_t *buffer, uint32_t length) {
    I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddr, false);
    I2CMasterDataPut(I2C0_BASE, startRegAddr);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy(I2C0_BASE));

    I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddr, true);

    if (length == 1) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
        while(I2CMasterBusy(I2C0_BASE));
        buffer[0] = I2CMasterDataGet(I2C0_BASE);
        return;
    }

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(I2CMasterBusy(I2C0_BASE));
    buffer[0] = I2CMasterDataGet(I2C0_BASE);

    for (uint32_t i = 1; i < length - 1; i++) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        while(I2CMasterBusy(I2C0_BASE));
        buffer[i] = I2CMasterDataGet(I2C0_BASE);
    }

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(I2CMasterBusy(I2C0_BASE));
    buffer[length - 1] = I2CMasterDataGet(I2C0_BASE);
}

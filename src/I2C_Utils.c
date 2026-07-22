#include "I2C_Utils.h"

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"

//======================================================
// Initialize I2C0
//======================================================

void I2C0_Init(void)
{
    // Enable peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    // Configure PB2=SCL PB3=SDA
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Standard Mode (100kHz)
    I2CMasterInitExpClk(I2C0_BASE,
                        SysCtlClockGet(),
                        false);
}

//======================================================
// Write One Register
//======================================================

bool I2C_WriteByte(uint8_t slaveAddr,
                   uint8_t regAddr,
                   uint8_t data)
{
    // Send Register Address
    I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddr, false);

    I2CMasterDataPut(I2C0_BASE, regAddr);

    I2CMasterControl(I2C0_BASE,
                     I2C_MASTER_CMD_BURST_SEND_START);

    while(I2CMasterBusy(I2C0_BASE));

    if(I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
        return false;

    // Send Data
    I2CMasterDataPut(I2C0_BASE, data);

    I2CMasterControl(I2C0_BASE,
                     I2C_MASTER_CMD_BURST_SEND_FINISH);

    while(I2CMasterBusy(I2C0_BASE));

    if(I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
        return false;

    return true;
}

//======================================================
// Read Multiple Bytes
//======================================================

bool I2C_ReadBurst(uint8_t slaveAddr,
                   uint8_t startRegAddr,
                   uint8_t *buffer,
                   uint32_t length)
{
    if(buffer == NULL || length == 0)
        return false;

    // Send Register Address
    I2CMasterSlaveAddrSet(I2C0_BASE,
                          slaveAddr,
                          false);

    I2CMasterDataPut(I2C0_BASE,
                     startRegAddr);

    I2CMasterControl(I2C0_BASE,
                     I2C_MASTER_CMD_SINGLE_SEND);

    while(I2CMasterBusy(I2C0_BASE));

    if(I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
        return false;

    // Restart in Read Mode
    I2CMasterSlaveAddrSet(I2C0_BASE,
                          slaveAddr,
                          true);

    // Single Byte Read
    if(length == 1)
    {
        I2CMasterControl(I2C0_BASE,
                         I2C_MASTER_CMD_SINGLE_RECEIVE);

        while(I2CMasterBusy(I2C0_BASE));

        if(I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
            return false;

        buffer[0] = I2CMasterDataGet(I2C0_BASE);

        return true;
    }

    // First Byte
    I2CMasterControl(I2C0_BASE,
                     I2C_MASTER_CMD_BURST_RECEIVE_START);

    while(I2CMasterBusy(I2C0_BASE));

    if(I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
        return false;

    buffer[0] = I2CMasterDataGet(I2C0_BASE);

    // Middle Bytes
    for(uint32_t i = 1; i < (length - 1); i++)
    {
        I2CMasterControl(I2C0_BASE,
                         I2C_MASTER_CMD_BURST_RECEIVE_CONT);

        while(I2CMasterBusy(I2C0_BASE));

        if(I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
            return false;

        buffer[i] = I2CMasterDataGet(I2C0_BASE);
    }

    // Last Byte
    I2CMasterControl(I2C0_BASE,
                     I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    while(I2CMasterBusy(I2C0_BASE));

    if(I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
        return false;

    buffer[length - 1] = I2CMasterDataGet(I2C0_BASE);

    return true;
}

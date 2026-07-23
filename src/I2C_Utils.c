#include "I2C_Utils.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"

//==================================================
// Global Driver Status
//==================================================

volatile I2C_Status_t g_i2cLastStatus = I2C_STATUS_OK;


//==================================================
// Private Helper Functions
//==================================================

/**
 * @brief Wait until the current I2C transaction completes.
 *
 * @return true if transaction completed successfully.
 * @return false on timeout or bus error.
 */
static bool I2C_WaitWhileBusy(void)
{
    uint32_t timeout = I2C_TIMEOUT_COUNT;

    while (I2CMasterBusy(I2C0_BASE))
    {
        if (--timeout == 0U)
        {
            g_i2cLastStatus = I2C_STATUS_TIMEOUT;
            return false;
        }
    }

    if (I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
    {
        g_i2cLastStatus = I2C_STATUS_BUS_ERROR;
        return false;
    }

    g_i2cLastStatus = I2C_STATUS_OK;
    return true;
}


//==================================================
// Initialize I2C0
//==================================================

void I2C0_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0))
    {
    }

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
    {
    }

    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Standard Mode (100 kHz)
    I2CMasterInitExpClk(
        I2C0_BASE,
        SysCtlClockGet(),
        false);

    g_i2cLastStatus = I2C_STATUS_OK;
}


//==================================================
// Write One Register
//==================================================

bool I2C_WriteByte(uint8_t slaveAddr,
                   uint8_t regAddr,
                   uint8_t data)
{
    I2CMasterSlaveAddrSet(I2C0_BASE, slaveAddr, false);

    I2CMasterDataPut(I2C0_BASE, regAddr);

    I2CMasterControl(
        I2C0_BASE,
        I2C_MASTER_CMD_BURST_SEND_START);

    if (!I2C_WaitWhileBusy())
    {
        return false;
    }

    I2CMasterDataPut(I2C0_BASE, data);

    I2CMasterControl(
        I2C0_BASE,
        I2C_MASTER_CMD_BURST_SEND_FINISH);

    return I2C_WaitWhileBusy();
}


//==================================================
// Read Multiple Registers
//==================================================

bool I2C_ReadBurst(uint8_t slaveAddr,
                   uint8_t startRegAddr,
                   uint8_t *buffer,
                   uint32_t length)
{
    if ((buffer == NULL) || (length == 0U))
    {
        g_i2cLastStatus = I2C_STATUS_INVALID_PARAMETER;
        return false;
    }

    //--------------------------------------------------
    // Send Register Address
    //--------------------------------------------------

    I2CMasterSlaveAddrSet(I2C0_BASE,
                          slaveAddr,
                          false);

    I2CMasterDataPut(I2C0_BASE,
                     startRegAddr);

    I2CMasterControl(I2C0_BASE,
                     I2C_MASTER_CMD_SINGLE_SEND);

    if (!I2C_WaitWhileBusy())
    {
        return false;
    }

    //--------------------------------------------------
    // Restart in Read Mode
    //--------------------------------------------------

    I2CMasterSlaveAddrSet(I2C0_BASE,
                          slaveAddr,
                          true);

    //--------------------------------------------------
    // Single Byte Read
    //--------------------------------------------------

    if (length == 1U)
    {
        I2CMasterControl(I2C0_BASE,
                         I2C_MASTER_CMD_SINGLE_RECEIVE);

        if (!I2C_WaitWhileBusy())
        {
            return false;
        }

        buffer[0] = I2CMasterDataGet(I2C0_BASE);
        return true;
    }

    //--------------------------------------------------
    // First Byte
    //--------------------------------------------------

    I2CMasterControl(I2C0_BASE,
                     I2C_MASTER_CMD_BURST_RECEIVE_START);

    if (!I2C_WaitWhileBusy())
    {
        return false;
    }

    buffer[0] = I2CMasterDataGet(I2C0_BASE);

    //--------------------------------------------------
    // Middle Bytes
    //--------------------------------------------------

    for (uint32_t i = 1U; i < (length - 1U); i++)
    {
        I2CMasterControl(I2C0_BASE,
                         I2C_MASTER_CMD_BURST_RECEIVE_CONT);

        if (!I2C_WaitWhileBusy())
        {
            return false;
        }

        buffer[i] = I2CMasterDataGet(I2C0_BASE);
    }

    //--------------------------------------------------
    // Last Byte
    //--------------------------------------------------

    I2CMasterControl(I2C0_BASE,
                     I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

    if (!I2C_WaitWhileBusy())
    {
        return false;
    }

    buffer[length - 1U] = I2CMasterDataGet(I2C0_BASE);

    return true;
}


//==================================================
// Driver Status
//==================================================

I2C_Status_t I2C_GetLastStatus(void)
{
    return g_i2cLastStatus;
}

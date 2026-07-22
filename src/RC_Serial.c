#include "RC_Serial.h"
#include "config.h"

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"

//==================================================
// GLOBAL VARIABLES
//==================================================

volatile RC_Commands_t g_rcCommands =
{
    ESC_MIN_US,
    0,
    0,
    0,
    0
};

volatile bool g_newRCDataAvailable = false;

volatile uint32_t g_systemTimeMs = 0;
volatile uint32_t g_lastPacketTimeMs = 0;


//==================================================
// PARSER STATE
//==================================================

typedef enum
{
    STATE_WAIT_START,
    STATE_READ_DATA

} ParseState_t;

static ParseState_t parserState = STATE_WAIT_START;

static uint8_t rxBuffer[10];
static uint8_t rxIndex = 0;


//==================================================
// SYSTICK TIMER
//==================================================

void SysTick_TimeBase_Init(void)
{
    SysTickPeriodSet(SysCtlClockGet() / 1000U);

    IntRegister(FAULT_SYSTICK, SysTickIntHandler);

    SysTickIntEnable();
    SysTickEnable();
}

void SysTickIntHandler(void)
{
    g_systemTimeMs++;
}


//==================================================
// UART INITIALIZATION
//==================================================

void RC_Serial_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);

    GPIOPinTypeUART(GPIO_PORTB_BASE,
                    GPIO_PIN_0 | GPIO_PIN_1);

    UARTConfigSetExpClk(
            UART1_BASE,
            SysCtlClockGet(),
            RC_BAUD_RATE,
            UART_CONFIG_WLEN_8 |
            UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);

    UARTFIFOEnable(UART1_BASE);

    UARTIntRegister(UART1_BASE, UART1_IntHandler);

    UARTIntEnable(UART1_BASE,
                  UART_INT_RX |
                  UART_INT_RT);

    IntEnable(INT_UART1);

    UARTEnable(UART1_BASE);
}


//==================================================
// UART INTERRUPT
//==================================================

void UART1_IntHandler(void)
{
    uint32_t status;

    status = UARTIntStatus(UART1_BASE, true);

    UARTIntClear(UART1_BASE, status);

    while(UARTCharsAvail(UART1_BASE))
    {
        uint8_t byte =
                (uint8_t)UARTCharGetNonBlocking(UART1_BASE);

        switch(parserState)
        {
            //--------------------------------------------------
            case STATE_WAIT_START:

                if(byte == PACKET_START_BYTE)
                {
                    rxIndex = 0;
                    parserState = STATE_READ_DATA;
                }

                break;

            //--------------------------------------------------
            case STATE_READ_DATA:

                if(rxIndex < sizeof(rxBuffer))
                {
                    rxBuffer[rxIndex++] = byte;
                }

                if(rxIndex >= sizeof(rxBuffer))
                {
                    uint8_t checksum = 0;

                    for(uint8_t i=0;i<9;i++)
                        checksum += rxBuffer[i];

                    if(checksum == rxBuffer[9])
                    {
                        g_rcCommands.throttle =
                                ((uint16_t)rxBuffer[0] << 8) |
                                 rxBuffer[1];

                        g_rcCommands.roll =
                                (int16_t)(((uint16_t)rxBuffer[2] << 8) |
                                           rxBuffer[3]);

                        g_rcCommands.pitch =
                                (int16_t)(((uint16_t)rxBuffer[4] << 8) |
                                           rxBuffer[5]);

                        g_rcCommands.yaw =
                                (int16_t)(((uint16_t)rxBuffer[6] << 8) |
                                           rxBuffer[7]);

                        g_rcCommands.armed =
                                rxBuffer[8];

                        g_newRCDataAvailable = true;

                        g_lastPacketTimeMs = g_systemTimeMs;
                    }

                    rxIndex = 0;
                    parserState = STATE_WAIT_START;
                }

                break;

            //--------------------------------------------------
            default:

                parserState = STATE_WAIT_START;
                rxIndex = 0;

                break;
        }
    }
}


//==================================================
// FAILSAFE
//==================================================

bool RC_Is_Connection_Safe(void)
{
    if((g_systemTimeMs - g_lastPacketTimeMs) > RC_TIMEOUT_MS)
    {
        g_rcCommands.throttle = ESC_MIN_US;
        g_rcCommands.armed = 0;

        return false;
    }

    return true;
}

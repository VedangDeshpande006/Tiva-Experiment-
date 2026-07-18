#include "RC_Serial.h"
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

// Initialize with safe defaults (Throttle low, Disarmed)
volatile RC_Commands_t g_rcCommands = {1000, 0, 0, 0, 0};
volatile bool g_newRCDataAvailable = false;
volatile uint32_t g_lastPacketTimeMs = 0;
volatile uint32_t g_systemTimeMs = 0; // incremented every 1ms by SysTick

// State machine for parsing incoming serial bytes
typedef enum { STATE_WAIT_START, STATE_READ_DATA } ParseState_t;
static ParseState_t parseState = STATE_WAIT_START;

// FIX: was uint8_t dataBuffer[9], but the parser writes indices 0-9
// (9 payload bytes + 1 checksum byte = 10 bytes) and read dataBuffer[9]
// for the checksum -- that was a 1-byte buffer overflow. Now sized to 10.
static uint8_t dataBuffer[10];
static uint8_t bufferIndex = 0;

void RC_Serial_Init(void) {
    // Enable UART1 and Port B
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Map PB0 to U1RX and PB1 to U1TX
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Set Baud Rate to 115200, 8 data bits, no parity, 1 stop bit
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    // Enable UART RX Interrupts
    IntRegister(INT_UART1, UART1_IntHandler);
    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
}

// FIX: g_lastPacketTimeMs was declared but never written anywhere in the
// codebase, so RC_Is_Connection_Safe could never behave correctly. This sets
// up a simple 1kHz SysTick tick to give the failsafe a real time source.
// Call SysTick_TimeBase_Init() once during startup, before the control loop.
void SysTick_TimeBase_Init(void) {
    SysTickPeriodSet(SysCtlClockGet() / 1000); // fire every 1ms
    IntRegister(FAULT_SYSTICK, SysTickIntHandler);
    SysTickIntEnable();
    SysTickEnable();
}

void SysTickIntHandler(void) {
    g_systemTimeMs++;
}

void UART1_IntHandler(void) {
    uint32_t ui32Status = UARTIntStatus(UART1_BASE, true);
    UARTIntClear(UART1_BASE, ui32Status);

    // Read all available characters from the hardware FIFO buffer
    while (UARTCharsAvail(UART1_BASE)) {
        uint8_t byte = UARTCharGetNonBlocking(UART1_BASE);

        if (parseState == STATE_WAIT_START) {
            if (byte == PACKET_START_BYTE) {
                bufferIndex = 0;
                parseState = STATE_READ_DATA;
            }
        }
        else if (parseState == STATE_READ_DATA) {
            dataBuffer[bufferIndex++] = byte;

            // Wait for 9 payload bytes + 1 checksum byte
            if (bufferIndex >= 10) {
                uint8_t receivedChecksum = dataBuffer[9];
                uint8_t calculatedChecksum = 0;

                // Sum the first 9 bytes to verify data integrity
                for (uint8_t i = 0; i < 9; i++) {
                    calculatedChecksum += dataBuffer[i];
                }

                // If the packet is clean, extract the values
                if (calculatedChecksum == receivedChecksum) {
                    g_rcCommands.throttle = (dataBuffer[0] << 8) | dataBuffer[1];
                    g_rcCommands.roll  = (int16_t)((dataBuffer[2] << 8) | dataBuffer[3]);
                    g_rcCommands.pitch = (int16_t)((dataBuffer[4] << 8) | dataBuffer[5]);
                    g_rcCommands.yaw   = (int16_t)((dataBuffer[6] << 8) | dataBuffer[7]);
                    g_rcCommands.armed = dataBuffer[8];
                    g_newRCDataAvailable = true;

                    // FIX: this was previously just a comment. Now actually
                    // stamps the time of the last good packet so the
                    // failsafe below can detect a stale/dead link.
                    g_lastPacketTimeMs = g_systemTimeMs;
                }
                parseState = STATE_WAIT_START;
            }
        }
    }
}

// Safety Failsafe: Call this continuously in the main control loop
bool RC_Is_Connection_Safe(uint32_t currentSystemTimeMs) {
    if ((currentSystemTimeMs - g_lastPacketTimeMs) > 500) {
        // Comm loss detected! Force motors to idle and disarm.
        g_rcCommands.throttle = 1000;
        g_rcCommands.armed = 0;
        return false;
    }
    return true;
}

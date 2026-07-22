#ifndef RC_SERIAL_H
#define RC_SERIAL_H

#include <stdint.h>
#include <stdbool.h>

//==================================================
// SERIAL PROTOCOL CONFIGURATION
//==================================================

#define PACKET_START_BYTE    0xAA
#define RC_PACKET_SIZE       10U

//==================================================
// RC COMMAND STRUCTURE
//==================================================

typedef struct
{
    uint16_t throttle;   // 1000 - 2000 us
    int16_t  roll;       // Desired roll command
    int16_t  pitch;      // Desired pitch command
    int16_t  yaw;        // Desired yaw rate
    uint8_t  armed;      // 0 = Disarmed, 1 = Armed

} RC_Commands_t;

//==================================================
// GLOBAL VARIABLES
//==================================================

extern volatile RC_Commands_t g_rcCommands;

extern volatile bool g_newRCDataAvailable;

extern volatile uint32_t g_lastPacketTimeMs;

extern volatile uint32_t g_systemTimeMs;

//==================================================
// PUBLIC API
//==================================================

/**
 * @brief Initializes UART1 for RC serial communication.
 */
void RC_Serial_Init(void);

/**
 * @brief Initializes the 1ms SysTick time base.
 */
void SysTick_TimeBase_Init(void);

/**
 * @brief UART1 interrupt handler.
 */
void UART1_IntHandler(void);

/**
 * @brief SysTick interrupt handler.
 */
void SysTickIntHandler(void);

/**
 * @brief Checks whether RC communication is still valid.
 *
 * @param currentSystemTimeMs Current system uptime in milliseconds.
 *
 * @return true  RC link is healthy.
 * @return false RC link timeout occurred.
 */
bool RC_Is_Connection_Safe(uint32_t currentSystemTimeMs);

#endif /* RC_SERIAL_H */

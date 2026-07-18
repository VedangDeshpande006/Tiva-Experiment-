#ifndef _RC_SERIAL_H_
#define _RC_SERIAL_H_

#include <stdint.h>
#include <stdbool.h>

// Unique byte to signal the start of a command packet
#define PACKET_START_BYTE 0xFC

// The structure of the incoming Xbox controller data
typedef struct {
    uint16_t throttle; // Range: 1000 to 2000
    int16_t  roll;     // Angle/Rate: -45 to +45
    int16_t  pitch;    // Angle/Rate: -45 to +45
    int16_t  yaw;      // Angle/Rate: -180 to +180
    uint8_t  armed;    // 0 = Motors Off, 1 = Armed
} RC_Commands_t;

// Global accessible command state
extern volatile RC_Commands_t g_rcCommands;
extern volatile bool g_newRCDataAvailable;
extern volatile uint32_t g_lastPacketTimeMs;
extern volatile uint32_t g_systemTimeMs; // millisecond counter, driven by SysTick

// Core functions
void RC_Serial_Init(void);
void UART1_IntHandler(void);
void SysTick_TimeBase_Init(void);   // call once from main() before the control loop
void SysTickIntHandler(void);       // register this as the SysTick ISR
bool RC_Is_Connection_Safe(uint32_t currentSystemTimeMs);

#endif /* _RC_SERIAL_H_ */

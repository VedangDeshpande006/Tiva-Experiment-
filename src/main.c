#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

// System and Configuration Headers
#include "config.h"
#include "I2C_Utils.h"
#include "Motors.h"
#include "RC_Serial.h"

// Sensor, Filtering, and State Headers
#include "FlightState.h"
#include "GY87.h"
#include "Kalman.h"

// Instantiate separate Kalman structures for the two spatial axes
Kalman_t kalmanRoll;
Kalman_t kalmanPitch;

// Initialize core I2C line and GY-87 10-DOF sensors
void FlightController_SensorsInit(void) {
    I2C0_Init();       // Spin up the physical peripheral wires
    GY87_Init();       // Wake up MPU6050, setup bypass, and init HMC5883L
    
    Kalman_init(&kalmanRoll);
    Kalman_init(&kalmanPitch);
} 

int main(void) {
    // 1. Set System Clock to 80MHz 
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // 2. Initialize Communications & Timebases
    RC_Serial_Init();        // Initialize UART1 for RC Receiver
    SysTick_TimeBase_Init(); // Initialize 1ms tick for connection failsafe

    // 3. Initialize Low-Level Hardware Drivers (Motors)
    Motors_Init();

    // 4. Initialize Sensors and Filters
    FlightController_SensorsInit();
    
    // 5. Run the Safe ESC Arming Sequence (Transmits 1000us floor pulse)
    Motors_Arm();

    // 6. Enable Interrupts Globally
    // IntMasterEnable(); 

    while(1) {
        // --- 250Hz Deterministic Main Loop Core Frame ---
        // TODO: Await timer tick (4ms flag or ISR sync)
        
        // Step A: Read GY87 Data & Update Kalman Filters
        // Step B: Check RC_Is_Connection_Safe() & Ingest Input Packets
        // Step C: Execute Attitude & Rate PID Calculations
        // Step D: Mix Actuator Outputs & Update Timers via Motors_SetWidths()
    }
}

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"

// Motor and Configuration Headers
#include "Motors.h"
#include "config.h"

// Sensor, Filtering, and State Headers
#include "FlightState.h"
#include "MPU6050.h"
#include "Kalman.h"

// Instantiate separate Kalman structures for the two spatial axes
Kalman_t kalmanRoll;
Kalman_t kalmanPitch;

// IMPORTANT: Call this once during your Tiva setup routine before the main loop!
void FlightController_SensorsInit(void) {
    // Note: Ensure I2C0_Init() and MPU6050_Init() are defined in your driver files
    I2C0_Init();       // Spin up the physical peripheral wires
    MPU6050_Init();    // Wake up chip and set the 20Hz vibration filter
    
    Kalman_init(&kalmanRoll);
    Kalman_init(&kalmanPitch);
} 

int main(void) {
    // 1. Set System Clock to 80MHz 
    // (Required for precise 1.25MHz PWM base tick rate and I2C timing)
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    // 2. Initialize Low-Level Hardware Drivers (Motors)
    Motors_Init();

    // 3. Initialize Sensors and Filters (I2C, MPU6050, Kalman)
    FlightController_SensorsInit();
    
    // 4. Run the Safe ESC Arming Sequence
    // Transmits 1000us floor pulse to let SimonK controllers calibrate their zero-point
    Motors_Arm();

    // 5. Future Global Interrupt/Peripheral Config Location
    // IntMasterEnable(); 

    while(1) {
        // --- 250Hz Deterministic Main Loop Core Frame ---
        // TODO: Await timer tick (4ms flag or ISR sync)
        
        // Step A: Read MPU6050 Data & Update Kalman Filters
        // Step B: Ingest RC Controller Input Packets
        // Step C: Execute Attitude & Rate PID Calculations
        // Step D: Mix Actuator Outputs & Update Timers via Motors_SetWidths()
    }
}

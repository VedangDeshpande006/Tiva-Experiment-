#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"   // <-- Required for IntMasterEnable()

// =====================================================
// Project Headers
// =====================================================
#include "config.h"
#include "I2C_Utils.h"
#include "Motors.h"
#include "RC_Serial.h"

#include "FlightState.h"
#include "GY87.h"
#include "Kalman.h"

// =====================================================
// Kalman Filter Instances
// =====================================================
Kalman_t kalmanRoll;
Kalman_t kalmanPitch;

// =====================================================
// Sensor Initialization
// =====================================================
static bool FlightController_SensorsInit(void)
{
    // Initialize I2C peripheral
    I2C0_Init();

    // Initialize GY-87 (MPU6050 + HMC5883L)
    if (!GY87_Init())
    {
        return false;
    }

    // Initialize Kalman Filters
    Kalman_init(&kalmanRoll);
    Kalman_init(&kalmanPitch);

    return true;
}

// =====================================================
// Main
// =====================================================
int main(void)
{
    // -------------------------------------------------
    // 1. Configure System Clock (80 MHz)
    // -------------------------------------------------
    SysCtlClockSet(
        SYSCTL_SYSDIV_2_5 |
        SYSCTL_USE_PLL |
        SYSCTL_OSC_MAIN |
        SYSCTL_XTAL_16MHZ);

    // -------------------------------------------------
    // 2. Initialize Communications
    // -------------------------------------------------
    RC_Serial_Init();
    SysTick_TimeBase_Init();

    // -------------------------------------------------
    // 3. Initialize Motors
    // -------------------------------------------------
    Motors_Init();

    // -------------------------------------------------
    // 4. Initialize Sensors
    // -------------------------------------------------
    if (!FlightController_SensorsInit())
    {
        // Sensor initialization failed.
        // Stay here until the fault is resolved.
        while (1)
        {
            // TODO:
            // Blink onboard LED
            // or send UART debug message
        }
    }

    // -------------------------------------------------
    // 5. Safe ESC Arming
    // -------------------------------------------------
    Motors_Arm();

    // -------------------------------------------------
    // 6. Enable Global Interrupts
    // -------------------------------------------------
    IntMasterEnable();

    // -------------------------------------------------
    // 7. Main Control Loop
    // -------------------------------------------------
    while (1)
    {
        //------------------------------------------------
        // TODO:
        // Wait for 250 Hz scheduler flag
        //------------------------------------------------

        /*
        1. Read GY87 sensors
        2. Scale IMU values
        3. Update Kalman filters
        4. Read RC commands
        5. Execute Cascaded PID
        6. Motor Mixing
        7. Update ESC PWM
        */
    }
}

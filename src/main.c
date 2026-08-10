/******************************************************************************
 * File    : main.c
 * Target  : TM4C123GH6PM (Tiva C LaunchPad)
 * Purpose : Autonomous Quadcopter Flight Controller
 *
 * Current Stage:
 *  - System clock
 *  - UART debug
 *  - SysTick 250 Hz scheduler
 *  - Motor PWM initialization
 *  - GY-87 initialization
 *  - Non-blocking IMU calibration
 *  - Flight state management
 *
 * NOT YET IMPLEMENTED:
 *  - RC input
 *  - Kalman attitude estimation
 *  - PID controller
 *  - Motor mixer
 *  - Automatic arming
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"

#include "config.h"
#include "I2C_Utils.h"
#include "Motors.h"
#include "GY87.h"
#include "FlightState.h"

/*=============================================================================
 * Control Loop Timing
 *===========================================================================*/

volatile uint32_t g_tickCount = 0U;
volatile bool g_controlLoopReady = false;

/*=============================================================================
 * Sensor & Flight State
 *===========================================================================*/

GY87_State_t g_sensorState;
FlightState_t g_flightState;

/*=============================================================================
 * SysTick Interrupt Handler
 *
 * 250 Hz = 4 ms period
 *===========================================================================*/

void SysTick_Handler(void)
{
    g_tickCount++;
    g_controlLoopReady = true;
}

/*=============================================================================
 * SysTick Initialization
 *===========================================================================*/

static void SysTick_Init(void)
{
    uint32_t sysclock = SysCtlClockGet();
    uint32_t ticks = sysclock / SYSTICK_HZ;

    SysTickPeriodSet(ticks);
    SysTickIntEnable();
    SysTickEnable();
}

/*=============================================================================
 * UART Initialization
 *===========================================================================*/

static void UART_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {
    }

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {
    }

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    GPIOPinTypeUART(
        GPIO_PORTA_BASE,
        GPIO_PIN_0 | GPIO_PIN_1
    );

    UARTClockSourceSet(
        UART0_BASE,
        UART_CLOCK_PIOSC
    );

    UARTStdioConfig(
        0,
        115200,
        16000000
    );
}

/*=============================================================================
 * Sensor Initialization
 *===========================================================================*/

static bool FlightController_SensorsInit(void)
{
    I2C0_Init();

    if (!GY87_Init())
    {
        printf("ERROR: GY87 initialization failed!\n");
        return false;
    }

    printf("GY87 sensors initialized successfully.\n");

    return true;
}

/*=============================================================================
 * Force All Motors to Minimum
 *===========================================================================*/

static void Motors_SetMinimum(void)
{
    Motors_SetPWM(MOTOR1, ESC_MIN_US);
    Motors_SetPWM(MOTOR2, ESC_MIN_US);
    Motors_SetPWM(MOTOR3, ESC_MIN_US);
    Motors_SetPWM(MOTOR4, ESC_MIN_US);
}

/*=============================================================================
 * Main
 *===========================================================================*/

int main(void)
{
    /*---------------------------------------------------------------------
     * 1. System Clock
     *
     * TM4C123GH6PM
     * 80 MHz system clock
     *-------------------------------------------------------------------*/

    SysCtlClockSet(
        SYSCTL_SYSDIV_2_5 |
        SYSCTL_USE_PLL |
        SYSCTL_OSC_MAIN |
        SYSCTL_XTAL_16MHZ
    );

    /*---------------------------------------------------------------------
     * 2. UART
     *-------------------------------------------------------------------*/

    UART_Init();

    printf("\n");
    printf("================================================\n");
    printf("  Autonomous Quadcopter Flight Controller\n");
    printf("  Target: TM4C123GH6PM (Tiva C)\n");
    printf("================================================\n\n");

    /*---------------------------------------------------------------------
     * 3. SYSTEM_INIT
     *-------------------------------------------------------------------*/

    printf("[SYSTEM_INIT] Initializing hardware...\n\n");

    /*---------------------------------------------------------------------
     * 4. SysTick
     *-------------------------------------------------------------------*/

    printf("  [1] SysTick (250 Hz control loop)... ");

    SysTick_Init();

    printf("OK\n");

    /*---------------------------------------------------------------------
     * 5. Motors
     *-------------------------------------------------------------------*/

    printf("  [2] Motors (PWM)... ");

    Motors_Init();

    Motors_SetMinimum();

    printf("OK\n");

    /*---------------------------------------------------------------------
     * 6. GY-87
     *-------------------------------------------------------------------*/

    printf("  [3] Sensors (I2C, GY87)... ");

    if (!FlightController_SensorsInit())
    {
        printf("FAILED\n");
        printf("\nERROR: Cannot proceed without sensors.\n");

        Motors_SetMinimum();

        while (1)
        {
            SysCtlDelay(SysCtlClockGet() / 3);
        }
    }

    printf("OK\n");

    /*---------------------------------------------------------------------
     * 7. Flight State
     *-------------------------------------------------------------------*/

    printf("  [4] Flight state machine... ");

    FlightState_Init(&g_flightState);

    printf("OK\n");

    /*---------------------------------------------------------------------
     * DO NOT ARM MOTORS HERE
     *
     * Motors remain at ESC_MIN_US during calibration.
     *-------------------------------------------------------------------*/

    Motors_SetMinimum();

    printf("  [5] Motors: DISARMED / MINIMUM PWM\n\n");

    /*---------------------------------------------------------------------
     * 8. SYSTEM_INIT → SYSTEM_CALIBRATING
     *-------------------------------------------------------------------*/

    g_flightState.mode = SYSTEM_CALIBRATING;

    printf("[SYSTEM_CALIBRATING]\n");
    printf("  Keep the IMU completely stationary and level.\n");
    printf("  Discarding first 125 samples (~500 ms)...\n");
    printf("  Collecting 1000 samples (~4 seconds)...\n\n");

    printf("  Calibration: [");
    fflush(stdout);

    uint32_t lastProgress = 0U;

    /*---------------------------------------------------------------------
     * 9. Enable Global Interrupts
     *
     * SysTick will now generate the 250 Hz scheduler.
     *-------------------------------------------------------------------*/

    IntMasterEnable();

    /*---------------------------------------------------------------------
     * 10. Main Control Loop
     *-------------------------------------------------------------------*/

    while (1)
    {
        /*
         * Wait for the next 4 ms control-loop tick.
         */

        if (!g_controlLoopReady)
        {
            continue;
        }

        /*
         * Clear scheduler flag.
         */

        g_controlLoopReady = false;

        /*-------------------------------------------------------------
         * Read GY-87
         *-----------------------------------------------------------*/

        GY87_UpdateState(&g_sensorState);

        /*-------------------------------------------------------------
         * Calibration
         *-----------------------------------------------------------*/

        if (g_flightState.mode == SYSTEM_CALIBRATING)
        {
            if (FlightState_UpdateCalibration(
                    &g_flightState,
                    &g_sensorState.imu))
            {
                printf("]\n\n");

                printf(
                    "[SYSTEM_CALIBRATING -> SYSTEM_DISARMED]\n"
                );

                printf("Calibration complete!\n\n");

                printf("Gyroscope biases:\n");

                printf(
                    "  X: %10.2f LSB\n",
                    g_flightState.attitude.gyroBiasX
                );

                printf(
                    "  Y: %10.2f LSB\n",
                    g_flightState.attitude.gyroBiasY
                );

                printf(
                    "  Z: %10.2f LSB\n",
                    g_flightState.attitude.gyroBiasZ
                );

                printf("\nAccelerometer biases:\n");

                printf(
                    "  X: %10.2f LSB\n",
                    g_flightState.attitude.accelBiasX
                );

                printf(
                    "  Y: %10.2f LSB\n",
                    g_flightState.attitude.accelBiasY
                );

                printf(
                    "  Z: %10.2f LSB\n",
                    g_flightState.attitude.accelBiasZ
                );

                printf("\n");
                printf("================================================\n");
                printf("  SYSTEM DISARMED\n");
                printf("  Motors remain at minimum PWM.\n");
                printf("================================================\n\n");

                printf("Current development stage:\n");
                printf("  1. RC input             [PENDING]\n");
                printf("  2. Attitude estimation  [PENDING]\n");
                printf("  3. PID controller       [PENDING]\n");
                printf("  4. Motor mixer          [PENDING]\n");
                printf("  5. Flight arming        [PENDING]\n\n");
            }
            else
            {
                uint32_t progress =
                    g_flightState.attitude.calibrationSamplesCollected;

                if (progress != lastProgress)
                {
                    if ((progress % 100U) == 0U)
                    {
                        printf(".");
                        fflush(stdout);
                    }

                    lastProgress = progress;
                }
            }

            /*
             * Safety:
             * Motors MUST remain at minimum during calibration.
             */

            Motors_SetMinimum();
        }

        /*-------------------------------------------------------------
         * DISARMED
         *-----------------------------------------------------------*/

        else if (g_flightState.mode == SYSTEM_DISARMED)
        {
            Motors_SetMinimum();

            /*
             * Print status once every second.
             *
             * 250 ticks = 1 second at 250 Hz.
             */

            if ((g_tickCount % 250U) == 0U)
            {
                printf(
                    "Status: DISARMED | IMU=%s | MAG=%s\n",
                    g_sensorState.imuHealthy ? "OK" : "FAIL",
                    g_sensorState.magnetometerHealthy ? "OK" : "FAIL"
                );
            }
        }

        /*-------------------------------------------------------------
         * ARMED
         *
         * Not implemented yet.
         *-----------------------------------------------------------*/

        else if (g_flightState.mode == SYSTEM_ARMED)
        {
            /*
             * DO NOT fly yet.
             *
             * PID, mixer and RC safety logic are not implemented.
             */

            Motors_SetMinimum();
        }

        /*-------------------------------------------------------------
         * FAILSAFE
         *-----------------------------------------------------------*/

        else if (g_flightState.mode == SYSTEM_FAILSAFE)
        {
            Motors_SetMinimum();

            printf(
                "FAILSAFE: Motors forced to minimum.\n"
            );
        }
    }

    return 0;
}

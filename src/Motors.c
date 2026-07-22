#include "Motors.h"
#include "config.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

void Motors_Init(void) {
    // 1. Enable peripherals (PWM0, PWM1, GPIOB)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // 2. Configure PB4, PB5, PB6, PB7 as PWM pins
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinConfigure(GPIO_PB7_M0PWM1);
    GPIOPinConfigure(GPIO_PB4_M0PWM2);
    GPIOPinConfigure(GPIO_PB5_M0PWM3);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    // 3. Set PWM clock generator based on system clock and PWM_FREQUENCY config
    // Add exact generator configuration based on your system clock setup here.
}

void Motors_SetPulse(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
    // Wrapper to update timer match values
}

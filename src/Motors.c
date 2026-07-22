#include "Motors.h"
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"

// Hardware Timer Constants
#define PWM_PERIOD_TICKS 3125
#define TICKS_PER_US     1.25 

void Motors_Init(void) {
    // 1. Enable PWM0 and GPIOB Peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    
    // Wait for peripherals to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0) || !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)) {}

    // 2. Set PWM clock divider to 64 (80MHz / 64 = 1.25MHz)
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // 3. Configure Pin Muxing for PB4-PB7
    GPIOPinConfigure(GPIO_PB6_M0PWM0); // Motor 1
    GPIOPinConfigure(GPIO_PB7_M0PWM1); // Motor 2
    GPIOPinConfigure(GPIO_PB4_M0PWM2); // Motor 3
    GPIOPinConfigure(GPIO_PB5_M0PWM3); // Motor 4

    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    // 4. Configure PWM Generators (Gen 0 handles PWM0/1, Gen 1 handles PWM2/3)
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // 5. Set the 400Hz Period
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, PWM_PERIOD_TICKS);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, PWM_PERIOD_TICKS);

    // 6. Initialize outputs to 1000us (Minimum Throttle / Disarmed)
    Motors_SetWidths(1000, 1000, 1000, 1000);

    // 7. Enable the Generators and Outputs
    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT | PWM_OUT_2_BIT | PWM_OUT_3_BIT, true);
}

void Motors_SetWidths(uint16_t m1_us, uint16_t m2_us, uint16_t m3_us, uint16_t m4_us) {
    // Safety bounds: constrain signals between 1000us and 2000us
    if(m1_us < 1000) m1_us = 1000; else if(m1_us > 2000) m1_us = 2000;
    if(m2_us < 1000) m2_us = 1000; else if(m2_us > 2000) m2_us = 2000;
    if(m3_us < 1000) m3_us = 1000; else if(m3_us > 2000) m3_us = 2000;
    if(m4_us < 1000) m4_us = 1000; else if(m4_us > 2000) m4_us = 2000;

    // Convert microseconds to hardware ticks and update duty cycle
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, (uint32_t)(m1_us * TICKS_PER_US)); 
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, (uint32_t)(m2_us * TICKS_PER_US)); 
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, (uint32_t)(m3_us * TICKS_PER_US)); 
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, (uint32_t)(m4_us * TICKS_PER_US)); 
}

void Motors_Arm(void) {
    Motors_SetWidths(1000, 1000, 1000, 1000);
    // Delay to allow ESCs to register the low signal and initialize
    SysCtlDelay(SysCtlClockGet() / 3); 
}

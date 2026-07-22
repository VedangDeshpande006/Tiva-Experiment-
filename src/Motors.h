#ifndef MOTORS_H_
#define MOTORS_H_

#include <stdint.h>

// Initializes PWM on PB4, PB5, PB6, PB7 at 400Hz
void Motors_Init(void);

// Sets individual motor pulse widths in microseconds (1000us to 2000us)
void Motors_SetWidths(uint16_t m1_us, uint16_t m2_us, uint16_t m3_us, uint16_t m4_us);

// Arms the ESCs by sending the minimum throttle signal
void Motors_Arm(void);

#endif /* MOTORS_H_ */

#ifndef _MOTORS_H_
#define _MOTORS_H_

#include <stdint.h>

void Motors_Init(void);
void Motors_SetPulse(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);

#endif

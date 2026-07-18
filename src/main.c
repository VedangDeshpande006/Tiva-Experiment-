#include "FlightState.h"
#include "MPU6050.h"
#include "Kalman.h"

// Instantiate separate Kalman structures for the two spatial axes
Kalman_t kalmanRoll;
Kalman_t kalmanPitch;

// IMPORTANT: Call this once during your Tiva setup routine before the main loop!
void FlightController_SensorsInit(void) {
    I2C0_Init();       // Spin up the physical peripheral wires
    MPU6050_Init();    // Wake up chip and set the 20Hz vibration filter
    
    Kalman_init(&kalmanRoll);
    Kalman_init(&kalmanPitch);
}

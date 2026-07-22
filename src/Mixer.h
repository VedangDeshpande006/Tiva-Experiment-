#ifndef MIXER_H
#define MIXER_H

#include <stdint.h>
#include "FlightState.h"

//==================================================
// QUADCOPTER X CONFIGURATION MOTOR MIXER
//==================================================
//
//           Front
//
//        M1(CCW)   M2(CW)
//
//             ^
//
//        M4(CW)    M3(CCW)
//
//            Rear
//
// Mixing Equations:
//
// M1 = Throttle + Roll - Pitch - Yaw
// M2 = Throttle - Roll - Pitch + Yaw
// M3 = Throttle - Roll + Pitch - Yaw
// M4 = Throttle + Roll + Pitch + Yaw
//
//==================================================

/**
 * @brief Mix PID outputs into motor commands.
 *
 * Combines throttle with Roll, Pitch and Yaw PID outputs
 * to generate the four ESC pulse widths for an X-frame
 * quadcopter.
 *
 * Motor outputs are automatically limited between
 * ESC_MIN_US and ESC_MAX_US.
 *
 * @param state     Pointer to FlightState structure.
 * @param rollCmd   Roll PID output.
 * @param pitchCmd  Pitch PID output.
 * @param yawCmd    Yaw PID output.
 */
void Mixer_Update(FlightState_t *state,
                  float rollCmd,
                  float pitchCmd,
                  float yawCmd);

#endif /* MIXER_H */

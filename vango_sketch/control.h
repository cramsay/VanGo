#ifndef CONTROL_H
#define CONTROL_H

#include "Arduino.h"
#include "tracking.h"

/* Constants for tweaking behaviour
 */
#define ACCURACY_LIMIT    2 // Resolution for "good enough" position
#define SPEED_LIMIT       7 // Max assignable speed
#define TIME_UNIT_MS      1 // Time in ms between control loop iterations
#define PWM_SPEED_FACTOR  6 // Scaling factor for our units and motor PWM values

/* comsInit:
 *  Must be called before any other control functions
 *  will operate correctly
 */
void ctrlInit(void);


/* ctrlToNextCoord(WheelPos *nextCoord)
 *  Takes the car iteratively towards nextCoord.
 *  Stops when within the resolution defined by ACCURACY_LIMIT
 */
void ctrlToNextCoord(WheelPos *nextCoord);

#endif

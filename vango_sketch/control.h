#ifndef CONTROL_H
#define CONTROL_H

#include "Arduino.h"
#include "tracking.h"

/* Constants for tweaking behaviour
 */
#define ACCURACY_LIMIT    2      // Resolution for "good enough" position
#define SPEED_LIMIT       20    // Max assignable speed
#define TIME_UNIT_MS      10   // Time in ms between control loop iterations
#define MAX_POS_SPEED     42  //The maximum speed the wheel could possibly turn at.
#define MAX_MOTOR_SETTING 50 //The maximum value the motor can be set to

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

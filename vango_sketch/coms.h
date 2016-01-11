#ifndef COMS_H
#define COMS_H

#include "Arduino.h"
#include "tracking.h"

/* comsInit:
 *  Must be called before any other coms functions
 *  will operate correctly
 */
void comsInit(void);


/* comsGetPos(WheelPos *pos)
 *  Blocking function which will give the next instruction recieved
 *  over Bluetooth. Usually from the Android app, but can be any device:
    very useful for testing or demo.
 */
void comsGetNextInstr(WheelPos *pos, int *penDown);

#endif

#ifndef TRACKING_H
#define TRACKING_H

#include "Arduino.h"


/* Structure for passing wheel velocity state
 */
typedef struct WheelVel{
  int x;
  int y;
} WheelVel;


/* Structure for passing wheel position state
 */
typedef struct WheelPos{
  int x1;
  int x2;
  int y1;
  int y2;
} WheelPos;


/* trackInit:
 *  Must be called before any other wheel tracking features
 *  will operate correctly
 */
void trackInit(void);


/* trackSetDir(int x1, int y1, int x2, int y2)
 *  Tells the module the direction the wheels are being turned
 *  This lets the tracking be as simple as possible
 *
 *  Directions control increment of positions and velocity signs.
 *  Any non-negative parameter is considered "forward", otherwise
 *  treated as "reverse"
 */
void trackSetDir(int x1, int y1, int x2, int y2);


/* trackGetPos(WheelPos *pos)
 *  Gets the wheel positions in the passed WheelPos structure
 */
void trackGetPos(WheelPos *pos);


/* trackGetVel(WheelVel *vel)
 *  Gets the wheel velocities in the passed WheelVel structure
 */
void trackGetVel(WheelVel *vel);

#endif

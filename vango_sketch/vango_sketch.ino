/*
 * At the moment this is how I see the main body of the 
 * code working. The areas enclosed in comments using /*
 * are what I intend to fill in myself and those using
 * // are currently there to leave in lines of code I'll
 * need to call from other peoples sections.
 */
#include "tracking.h"

#define accuracyLimit 2

WheelPos target ={0,0,0,0};
WheelPos current ={0,0,0,0};
WheelPos diff={0,0,0,0};

void setup() {
  trackInit();
  //adamStuff();
}

void loop() {
  //nextCoord(&target);
  trackGetPos(&current);
  diff=getDiff(target,current);
  int xdiff=(diff.x1+diff.x2)/2;
  int ydiff=(diff.y1+diff.y2)/2;
  while((xdiff>accuracyLimit)&&(ydiff>accuracyLimit)){
    /*
     * find ratio between x and y
     * find smallest applicable approximation of ratio using 
     * whole numbers basically
     * adjust to balance x and y while maintaining ratio so if 
     * x1 and x2 aren't the same this can be fixed
     * apply these values to motors
     * short loop
     * this loop is used to allow it to drive forward for a 
     * while adjusting the motor speeds at set intervals
     * to try to get them to what was intended accounting
     * for different environmental issues like them being better
     * in one direction than the other, etc
     *      wait very small amount of time
     *      check velocities
     *      adjust motors
     * check position
     * find average x difference
     * find average y difference
     */
  }
}

WheelPos getDiff(WheelPos target,WheelPos current){
  /*
   * Do stuff mainly subtraction I'd guess really but I'm
   * working out high level stuff just now.
   */
  return target;
}


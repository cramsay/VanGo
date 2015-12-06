/* Main loop
 *  
 *  As it stands, this is a wee demo of how to
 *  interface with the wheel tracking module.
 *  
 *  Prints wheel positions to serial every second.
 *  Hardware setup:
 *    2 x IR sensor
 *      -> Sensor D0's to pins 49 (x axis) and 48 (y axis)
 */

#include "tracking.h"

WheelPos pos = {0,0,0,0};

void setup() {
  Serial.begin(115200);
  
  trackInit();
  trackSetDir(1,1);
}

void loop() {
  trackGetPos(&pos);
  
  Serial.print(pos.x1);
  Serial.print(",");
  Serial.println(pos.y1);
  
  delay(1000);
}

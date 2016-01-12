#include "tracking.h"
#include "coms.h"
#include "control.h"
#include <Servo.h>

//The pin on the arduino that the servo has been connected to.
#define servoPin 8
//The servo setting to lift the pen.
#define penUp 180
//The servo setting to drop the pen
#define penDown 130

Servo penLift;
int penLocation;
WheelPos target;

void setup() {
  penLift.attach(servoPin);
  
  trackInit();
  comsInit();
  ctrlInit();
}

void loop() {
  comsGetNextInstr(&target,&penLocation);
  
  if(penLocation==1)
    penLift.write(penUp);
  else
    penLift.write(penDown);

  ctrlToNextCoord(&target);
}

#include "tracking.h"
#include "coms.h"
#include "control.h"
#include <Servo.h>

//The pin on the arduino that the servo has been connected to.
#define SERVO_PIN 9
//The servo setting to lift the pen.
#define PEN_UP 180
//The servo setting to drop the pen
#define PEN_DOWN 130

Servo pen;
int penLocation;
WheelPos target;

void setup() {
  trackInit();
  comsInit();
  ctrlInit();
  
  pen.attach(SERVO_PIN);
}

void loop() {
  comsGetNextInstr(&target,&penLocation);

  if(penLocation==1){
    pen.write(PEN_UP);
    Serial1.println("Pen going up");
  } else {
    pen.write(PEN_DOWN);
    Serial1.println("Pen going down");
  }

  ctrlToNextCoord(&target);
}

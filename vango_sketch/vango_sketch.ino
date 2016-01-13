#include "tracking.h"
#include "coms.h"
#include "control.h"
#include <Servo.h>

//The pin on the arduino that the servo has been connected to.
#define SERVO_PIN 9
//The servo setting to lift the pen.
#define PEN_UP 170
//The servo setting to drop the pen
#define PEN_DOWN 130

Servo pen;
int penDown,prevPenDown;
WheelPos target;

void setup() {
  trackInit();
  comsInit();
  ctrlInit();

  Serial.begin(115200);
  pen.attach(SERVO_PIN);
  pen.write(PEN_UP);
  penDown=0;
}

void loop() {
  comsGetNextInstr(&target,&penDown);

  if(penDown!=prevPenDown){
    if(penDown==1){
      pen.write(PEN_DOWN);
      Serial2.println("Pen going down");
    } else {
      pen.write(PEN_UP);
      Serial2.println("Pen going up");
    }
    prevPenDown = penDown;
    delay(700);
  }

  ctrlToNextCoord(&target);
}

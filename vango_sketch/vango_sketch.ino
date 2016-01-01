/*
 * At the moment this is how I see the main body of the 
 * code working. The areas enclosed in comments using /*
 * are what I intend to fill in myself and those using
 * // are currently there to leave in lines of code I'll
 * need to call from other peoples sections.
 * 
 * To Do List
 * Should be able to track both wheels on one axis in different directions.
 * Need to use double for diff&scale factor rather than int otherwise scale diff won't work.
 */
#include "tracking.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

#define accuracyLimit 2
#define motorLowerLimit 50
#define motorUpperLimit 150


typedef struct Motors{
  Adafruit_DCMotor* x1;
  Adafruit_DCMotor* x2;
  Adafruit_DCMotor* y1;
  Adafruit_DCMotor* y2;
}Motors;

Motors motors;
WheelPos target ={0,0,0,0};
WheelPos current ={0,0,0,0};
WheelPos diff={0,0,0,0};


void setup() {
  trackInit();
  //adamStuff();
  motors=motorInit();
}

Motors motorInit(){
  /*
   * Remember to check this against hardware set up to ensure
   * they match the sensor labels and are on the correct axis.
   */
  Adafruit_MotorShield AFMS = Adafruit_MotorShield();
  Motors motorStruct={AFMS.getMotor(1),AFMS.getMotor(3),
                      AFMS.getMotor(2),AFMS.getMotor(4)};
  return motorStruct;
}

void loop() {
  //nextCoord(&target);
  int largestDistance;
  updateDiff();
  while(diff.x1>accuracyLimit||diff.x2>accuracyLimit||diff.y1>accuracyLimit||diff.y2>accuracyLimit){
    setDirections();
    /* Remember you've already set the directions so at this stage
     *  you can just deal with magnitudes everywhere.
     */ 
    largestDistance=findMaxMagDiff();
    if(largestDistance>distanceInTimeUnitLimit){
      scaleDiff(distanceInTimeUnitLimit/largestDistance);
    }
    move();
    wait(timeUnit);
    updateDiff();
  }
}



//TODO need to fix last line to take 4 parameters
void setDirections(){
  if(diff.x1<0){
    motors.x1->run(BACKWARD);
  }else{
    motors.x1->run(FORWARD);
  }
  if(diff.x2<0){
    motors.x2->run(BACKWARD);
  }else{
    motors.x2->run(FORWARD);
  }
  if(diff.y1<0){
    motors.y1->run(BACKWARD);
  }else{
    motors.y1->run(FORWARD);
  }
  if(diff.y2<0){
    motors.y2->run(BACKWARD);
  }else{
    motors.y2->run(FORWARD);
  }
  trackSetDir(xDiff,yDiff);
}


void updateDiff(WheelPos target,WheelPos current){
   trackGetPos(&current);
   WheelPos diff = {0,0,0,0};
   diff.x1=target.x1-current.x1;
   diff.x1=target.x2-current.x2;
   diff.x1=target.y1-current.y1;
   diff.x1=target.y2-current.y2;
}

int findMaxMagDiff(){
  /*Run through each part of diff if it's negative 
   * make it positive then find the biggest out 
   * of them
   */
   if(diff.x1<0){
      diff.x1=diff.x1*-1;
   }
   if(diff.x2<0){
      diff.x2=diff.x2*-1;
   }
   if(diff.y1<0){
      diff.y1=diff.y1*-1;
   }
   if(diff.y2<0){
      diff.y2=diff.y2*-1;
   }
   if(diff.x1>=diff.x2&&diff.x1>=diff.y1&&diff.x1>=diff.y2){
      return diff.x1;
   }
   if(diff.x2>=diff.y1&&diff.x2>=y2){
      return diff.x2;
   }
   if(diff.y1>=diff.y2){
    return diff.y1;
   }
   return diff.y2; 
}

void scaleDiff(int scaleFactor){
  /* Multiply all four diffs by scale factor
   *  this will give you the largest being the 
   *  limit and the rest being appropriately scaled down.
   */
   diff.x1=diff.x1*scaleFactor;
   diff.x2=diff.x2*scaleFactor;
   diff.y1=diff.y1*scaleFactor;
   diff.y2=diff.y2*scaleFactor;
}


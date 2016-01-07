/*
 * At the moment this is how I see the main body of the 
 * code working. The areas enclosed in comments using /*
 * are what I intend to fill in myself and those using
 * // are currently there to leave in lines of code I'll
 * need to call from other peoples sections.
 * 
 * To Do List
 * Should be able to track both wheels on one axis in different directions.
 * 
 * Likely Problems checklist:
 * Make sure the motors are hooked up to the same wheels as the tracking thinks they are.
 * Passing a double into a method that takes an int in go.
 */
#include "tracking.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

#define accuracyLimit 2
#define speedLimit 40
#define timeUnit 1
#define maxPosSpeed 40



typedef struct Motors{
  Adafruit_DCMotor* x1;
  Adafruit_DCMotor* x2;
  Adafruit_DCMotor* y1;
  Adafruit_DCMotor* y2;
}Motors;

typedef struct Speeds{
  double x1;
  double x2;
  double y1;
  double y2;
}Speeds;

Motors motors;
WheelPos target ={0,0,0,0};
WheelPos current ={0,0,0,0};
Speeds idealSpeeds={0,0,0,0};


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
  double largestSpeed;
  updateIdealSpeeds();
  while(idealSpeeds.x1>accuracyLimit||idealSpeeds.x2>accuracyLimit||idealSpeeds.y1>accuracyLimit||idealSpeeds.y2>accuracyLimit){
    setDirections();
    /* Remember you've already set the directions so at this stage
     *  you can just deal with magnitudes everywhere.
     */ 
    largestSpeed=findMaxIdealSpeeds();
    if(largestSpeed>speedLimit){
      scaleIdealSpeeds(speedLimit/largestSpeed);
    }
    go();
    delay(timeUnit);
    updateIdealSpeeds();
  }
}



//TODO need to fix last line to take 4 parameters
void setDirections(){
  if(idealSpeeds.x1<0){
    motors.x1->run(BACKWARD);
  }else{
    motors.x1->run(FORWARD);
  }
  if(idealSpeeds.x2<0){
    motors.x2->run(BACKWARD);
  }else{
    motors.x2->run(FORWARD);
  }
  if(idealSpeeds.y1<0){
    motors.y1->run(BACKWARD);
  }else{
    motors.y1->run(FORWARD);
  }
  if(idealSpeeds.y2<0){
    motors.y2->run(BACKWARD);
  }else{
    motors.y2->run(FORWARD);
  }
//  trackSetDir(xDiff,yDiff);
}


void updateIdealSpeeds(){
   trackGetPos(&current);
   WheelPos idealSpeeds = {0,0,0,0};
   idealSpeeds.x1=target.x1-current.x1;
   idealSpeeds.x1=target.x2-current.x2;
   idealSpeeds.x1=target.y1-current.y1;
   idealSpeeds.x1=target.y2-current.y2;
}

int findMaxIdealSpeeds(){
  /*Run through each part of idealSpeeds if it's negative 
   * make it positive then find the biggest out 
   * of them
   */
   if(idealSpeeds.x1<0){
      idealSpeeds.x1=idealSpeeds.x1*-1;
   }
   if(idealSpeeds.x2<0){
      idealSpeeds.x2=idealSpeeds.x2*-1;
   }
   if(idealSpeeds.y1<0){
      idealSpeeds.y1=idealSpeeds.y1*-1;
   }
   if(idealSpeeds.y2<0){
      idealSpeeds.y2=idealSpeeds.y2*-1;
   }
   if(idealSpeeds.x1>=idealSpeeds.x2&&idealSpeeds.x1>=idealSpeeds.y1&&idealSpeeds.x1>=idealSpeeds.y2){
      return idealSpeeds.x1;
   }
   if(idealSpeeds.x2>=idealSpeeds.y1&&idealSpeeds.x2>=idealSpeeds.y2){
      return idealSpeeds.x2;
   }
   if(idealSpeeds.y1>=idealSpeeds.y2){
    return idealSpeeds.y1;
   }
   return idealSpeeds.y2; 
}

void scaleIdealSpeeds(double scaleFactor){
  /* Multiply all four idealSpeedss by scale factor
   *  this will give you the largest being the 
   *  limit and the rest being appropriately scaled down.
   */
   idealSpeeds.x1=idealSpeeds.x1*scaleFactor;
   idealSpeeds.x2=idealSpeeds.x2*scaleFactor;
   idealSpeeds.y1=idealSpeeds.y1*scaleFactor;
   idealSpeeds.y2=idealSpeeds.y2*scaleFactor;
}

void go(){
  motors.x1->setSpeed((idealSpeeds.x1/maxPosSpeed)*250);
  motors.x2->setSpeed((idealSpeeds.x2/maxPosSpeed)*250);
  motors.y1->setSpeed((idealSpeeds.y1/maxPosSpeed)*250);
  motors.y2->setSpeed((idealSpeeds.y2/maxPosSpeed)*250);
}


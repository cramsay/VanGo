/*
 * At the moment this is how I see the main body of the 
 * code working. The areas enclosed in comments using /*
 * are what I intend to fill in myself and those using
 * // are currently there to leave in lines of code I'll
 * need to call from other peoples sections.
 * 
 * To Do List
 * Finish implmenting comments
 * Add ability to travel in negative directions
 * Integrate other code sections
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
int xDiff;
int yDiff;


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
  int xSpeed;
  int ySpeed;
  //nextCoord(&target);
  setDiff();
  while((xDiff>accuracyLimit)&&(yDiff>accuracyLimit)){
    /*
     * Working on beneath
     * find ratio between x and y
     * find smallest applicable approximation of ratio using 
     * whole numbers basically
     */
    setDirections();
    xSpeed=xDiff;
    ySpeed=yDiff;
    hcf=findHighestCommonFactor(xSpeed,ySpeed);
    if(hcf!=null){
      xSpeed=xSpeed/hcf;
      ySpeed=ySpeed/hcf;
    }
    setMotorSpeedX(xSpeed);
    setMotorSpeedY(ySpeed);
  /*
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
      */
    setDiff();
  }
}

//TODO stopped working here last time
void setDirections(){
  if(xDiff<0){
    motors.x1->run(BACKWARD);
    motors.x2->run(BACKWARD);
    
  }
}

void setDiff(){
  trackGetPos(&current);
  updateDiff(target,current);
  xDiff=(diff.x1+diff.x2)/2;
  yDiff=(diff.y1+diff.y2)/2;
}

void updateDiff(WheelPos target,WheelPos current){
  /*
   * Do stuff mainly subtraction I'd guess really but I'm
   * working out high level stuff just now.
   */
   WheelPos diff = {0,0,0,0};
   diff.x1=target.x1-current.x1;
   diff.x1=target.x2-current.x2;
   diff.x1=target.y1-current.y1;
   diff.x1=target.y2-current.y2;
}

int findHighestCommonFactor(int one,int two){
  int hcf=null;
  for(i=0;i<one||i<two;i++){
    if ((one%i==0)&&(two%i==0)){
      hcf=i;
    }
  }
  return hcf;
}


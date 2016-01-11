/*
 * Likely Problems checklist:
 * Make sure the motors are hooked up to the same wheels as the tracking thinks they are.
 */
#include "tracking.h"
#include "coms.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Servo.h>

//Smallest distance the car is capable of moving. By including this it stops
//the car from trying to edge back and forwards to get to an exact point it
//can never reach. 
#define accuracyLimit 2
//Maximum speed the car shhould be allowed to travel at.
#define speedLimit 4
//The length of time the car should be allowed to move before the actual
//speed is measured and the bias is adjusted.
#define timeUnit 1
//Maximum speed the car could possibly travel at. Used to create a ratio 
//between the maximum possible speed and the ideal speed for the car to
//travel at. This ratio can then be used to set the motors.
#define maxPosSpeed 40
//The pin on the arduino that the servo has been connected to.
#define servoPin 8
//The servo setting to lift the pen.
#define penUp 180
//The servo setting to drop the pen
#define penDown 130

Adafruit_MotorShield AFMS;

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

Servo penLift;
int penLocation;
Motors motors;
//Directional Structures
WheelPos target ={0,0,0,0};
WheelPos current ={0,0,0,0};


Speeds idealSpeeds={0,0,0,0};

//Bias Structures
/*
 * These are used throughout to try to adjust for 
 * inequalities between the motors. DC motors are
 * also usually better at turning in one direction
 * than the other hence the need for a bias in 
 * each direction.
 */
Speeds forwardBias = {0,0,0,0};
Speeds backwardBias={0,0,0,0};
Speeds currentDirection{0,0,0,0};


void setup() {
  Serial.begin(9600);
  trackInit();
  comsInit();
  motorInit();
}

void motorInit(){
  /*
   * Remember to check this against hardware set up to ensure
   * they match the sensor labels and are on the correct axis.
   */
  penLift.attach(servoPin);
  AFMS = Adafruit_MotorShield();
  motors={AFMS.getMotor(1),AFMS.getMotor(3),
                      AFMS.getMotor(2),AFMS.getMotor(4)};
  AFMS.begin();
}

void loop() {
  comsGetNextInstr(&target,&penLocation);
  
  if(penLocation==1){
    penLift.write(penUp);
  }else{
    penLift.write(penDown);
  }
  double largestSpeed;
  updateIdealSpeeds();
  while(idealSpeeds.x1>accuracyLimit||idealSpeeds.x2>accuracyLimit||idealSpeeds.y1>accuracyLimit||idealSpeeds.y2>accuracyLimit){
    setDirections();
    //After the directions have been accounted for only the magnitudes are important.
    largestSpeed=findMaxIdealSpeeds();
    //After the line above the ideal speeds are magnitude only.
    if(largestSpeed>speedLimit){
      scaleIdealSpeeds(speedLimit/largestSpeed);
    }
    go();
    delay(timeUnit);
    updateBias();
    updateIdealSpeeds();
  }
}

/*
 * Used to update either the forward or backward bias depending on the
 * direction of each wheel. 
 */
void updateBias(){
  WheelPos newPosition={0,0,0,0};
  trackGetPos(&newPosition);
  newPosition.x1=newPosition.x1-current.x1;
  newPosition.x2=newPosition.x2-current.x2;
  newPosition.y1=newPosition.y1-current.y1;
  newPosition.y2=newPosition.y2-current.y2;
  //newPosition now holds the actual speed in wheel segments per time unit.
  if(currentDirection.x1==1){
    forwardBias.x1=forwardBias.x1+(newPosition.x1-idealSpeeds.x1);
    //The bias is adjusted by the difference between the actual 
    //speed and the ideal speed to try to correct the actual speed.
  }else{
    backwardBias.x1=backwardBias.x1+(idealSpeeds.x1+newPosition.x1);
    //This has to be the ideal speed plus the actual speed here
    //as the actual speed should be negative where as by this stage
    //the ideal speed will be a magnitute and hence positive. 
    //This means when the actual speed is negative it becomes a
    //difference of and the correct adjustment is applied to the
    //bias. When the actual speed is positive this calculation is
    //still correct because the actual speed is off by the entire
    //ideal speed plus the actual speed because it is driving in
    //the wrong direction.
  }
  if(currentDirection.x2==1){
    forwardBias.x2=forwardBias.x2+(newPosition.x2-idealSpeeds.x2);
  }else{
    backwardBias.x2=backwardBias.x2+(idealSpeeds.x2+newPosition.x2);
    //Same as for x1
  }
  if(currentDirection.y1==1){
    forwardBias.y1=forwardBias.y1+(newPosition.y1-idealSpeeds.y1);
  }else{
    backwardBias.y1=backwardBias.y1+(idealSpeeds.y1+newPosition.y1);
    //Same as for x1
  }
  if(currentDirection.y2==1){
    forwardBias.y2=forwardBias.y2+(newPosition.y2-idealSpeeds.y2);
  }else{
    backwardBias.y2=backwardBias.y2+(idealSpeeds.y2+newPosition.y2);
    //Same as for x1
  }
}

/*
 * Sets the direction that each motor should be turning in.
 * Also stores this direction for use in the bias and informs
 * the tracking module of these directions.
 */
void setDirections(){
  if(idealSpeeds.x1<0){
    motors.x1->run(BACKWARD);
    currentDirection.x1=0;
  }else{
    motors.x1->run(FORWARD);
    currentDirection.x1=1;
  }
  if(idealSpeeds.x2<0){
    motors.x2->run(BACKWARD);
    currentDirection.x2=0;
  }else{
    motors.x2->run(FORWARD);
    currentDirection.x2=1;
  }
  if(idealSpeeds.y1<0){
    motors.y1->run(BACKWARD);
    currentDirection.y1=0;
  }else{
    motors.y1->run(FORWARD);
    currentDirection.y1=1;
  }
  if(idealSpeeds.y2<0){
    motors.y2->run(BACKWARD);
    currentDirection.y2=0;
  }else{
    motors.y2->run(FORWARD);
    currentDirection.y2=1;
  }
  trackSetDir(idealSpeeds.x1,idealSpeeds.y1,idealSpeeds.x2,idealSpeeds.y2);
}

/*
 * Uses the tracking section to find the current location and compares this
 * to the target to give a set of ideal speeds for each wheel.
 */
void updateIdealSpeeds(){
   trackGetPos(&current);
   idealSpeeds = {0,0,0,0};
   idealSpeeds.x1=target.x1-current.x1;
   idealSpeeds.x2=target.x2-current.x2;
   idealSpeeds.y1=target.y1-current.y1;
   idealSpeeds.y2=target.y2-current.y2;
}
/*
 * Returns the maximum ideal speed.
 * Also changes all of the ideal speeds into magnitudes.
 */
int findMaxIdealSpeeds(){
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

  /* 
   *  Multiply all four idealSpeeds by scale factor
   *  this will give you the largest being the 
   *  limit and the rest being appropriately scaled down.
   */
void scaleIdealSpeeds(double scaleFactor){
   idealSpeeds.x1=idealSpeeds.x1*scaleFactor;
   idealSpeeds.x2=idealSpeeds.x2*scaleFactor;
   idealSpeeds.y1=idealSpeeds.y1*scaleFactor;
   idealSpeeds.y2=idealSpeeds.y2*scaleFactor;
}

/*
 * This is the method that either starts the motors moving or adjusts the speed.
 */
void go(){
  if(currentDirection.x1==1){
    motors.x1->setSpeed((int)((idealSpeeds.x1+forwardBias.x1)/maxPosSpeed*250));
  }else{
    motors.x1->setSpeed((int)((idealSpeeds.x1+backwardBias.x1)/maxPosSpeed*250));
  }
  if(currentDirection.x2==1){
    motors.x2->setSpeed((int)((idealSpeeds.x2+forwardBias.x2)/maxPosSpeed*250));
  }else{
    motors.x2->setSpeed((int)((idealSpeeds.x2+backwardBias.x2)/maxPosSpeed*250));
  }
  if(currentDirection.y1==1){
    motors.y1->setSpeed((int)((idealSpeeds.y1+forwardBias.y1)/maxPosSpeed*250));
  }else{
    motors.y1->setSpeed((int)((idealSpeeds.y1+backwardBias.y1)/maxPosSpeed*250));
  }
  if(currentDirection.y2==1){
    motors.y2->setSpeed((int)((idealSpeeds.y2+forwardBias.y2)/maxPosSpeed*250));
  }else{
    motors.y2->setSpeed((int)((idealSpeeds.y2+backwardBias.y2)/maxPosSpeed*250));
  }
}


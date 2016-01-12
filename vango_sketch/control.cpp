/*
 * Motor control module:
 * 
 * This works with tracking.h to control the position of the system.
 * This is essentially a proportional controller with some accounting
 * for biases in the motors using historic feedback.
 *
 * TODO:
 *        + A biased motor should NOT be driven when idealSpeed == 0!
 *        + Off-centre wheels don't trigger sensor all the way around
 *          the rotation...
 */

/* Setup **************************************************************
 **********************************************************************/

#include "control.h"
#include "tracking.h"
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

/* Struct to hold the state of a motor
 */
typedef struct
{
  Adafruit_DCMotor* motor;  //Motor from adafruit library
  int startPos;             //Position at start of tick
  int endPos;               //Position at end of tick
  int targetPos;            //Position that we're aiming for
  float idealSpeed;         //Speed we'd like to drive at
  float biasedSpeed;        //Speed accounting for bias
  float bias[2];            //Biases (1 forward, 1 back)
} MotorState;

/* Indices into bias arrays
 */
#define BIAS_FORWARD 0
#define BIAS_BACKWARD 1

#define NUM_MOTORS 4
MotorState motors[NUM_MOTORS];  //Collection of all motors
Adafruit_MotorShield AFMS;      //Adafruit library driver

/* Function prototypes
 */
void updateBiases();
int  isAtTarget();
void updateIdealSpeeds();
void applyBiasedSpeeds();


/* Public functions ***************************************************
 **********************************************************************/

void ctrlInit(void) {
  int i;
  //Init Adafruit driver
  AFMS = Adafruit_MotorShield();

  //Init motors collection
  for(i=0;i<NUM_MOTORS;i++)
    motors[i].motor = AFMS.getMotor(i+1);

  //Enable motors
  AFMS.begin();
}


void ctrlToNextCoord(WheelPos *nextCoord){

  //Store target positions
  motors[X1].targetPos = nextCoord->x1;
  motors[Y1].targetPos = nextCoord->y1;
  motors[X2].targetPos = nextCoord->x2;
  motors[Y2].targetPos = nextCoord->y2;

  Serial1.println("Starting to hone in on new coord");
  
  //Loop while not at target
  while(!isAtTarget()){
    
    //Update ideal speeds
    Serial1.println("Updating ideal speeds");
    updateIdealSpeeds();
  
    //Drive motors with biased speeds to compensate for reality
    Serial1.println("Driving motors with biased speeds");
    applyBiasedSpeeds();
    
    //Wait for some movement
    delay(TIME_UNIT_MS);
    
    //Update the biases
    Serial1.println("Updating biases");
    updateBiases();
    
  }
  
}

/* Private functions **************************************************
 **********************************************************************/

 /* int isAtTarget()
  *  Works out current position relative to target position. If within
  *  a resolution of ACCURACY_LIMIT in all directions, we are close
  *  enough and return 1.
  *  
  *  Otherwise, we are not at the target and returns 0
  */
 int isAtTarget(){
  int i,error;

  //Update actual postitions
  WheelPos curs;
  trackGetPos(&curs);
  motors[X1].startPos = curs.x1;
  motors[Y1].startPos = curs.y1;
  motors[X2].startPos = curs.x2;
  motors[Y2].startPos = curs.y2;
  
  //Compare actual and target positions on all motors
  for(i=0;i<NUM_MOTORS;i++){
    error = motors[i].startPos - motors[i].targetPos;
    if(abs(error)>ACCURACY_LIMIT)
      return 0;
  }

  return 1;
 }

 /* void updateIdealSpeeds()
  *   Calculates the ideal speeds we should be driving at
  *   to reach the target. If any speed is found to be
  *   above SPEED_LIMIT then all speeds are scaled down to
  *   be in rage.
  */
 void updateIdealSpeeds(){
  int i;
  float highest;
  float scaleFactor = 1;

  //Keep looping until all speeds are sensible
  do{

    //Reset highest counter
    highest = 0;

    //Find ideal speed and check if this is a new highest
    for(i=0;i<NUM_MOTORS;i++){
      motors[i].idealSpeed = (motors[i].targetPos - motors[i].startPos) * scaleFactor;
      
      if(abs(motors[i].idealSpeed) > highest)
        highest = abs(motors[i].idealSpeed);
        
    }

    //Speculatively calculate scaling factor for next pass
    scaleFactor = SPEED_LIMIT/highest;
  }while(highest>SPEED_LIMIT);
  
 }
 
/* void applyBiasedSpeeds()
 *  Applies each motor's directional bias to the ideal speed
 *  and drives the motor in the right direction.
 */
 void applyBiasedSpeeds(){
  int i,biasIndex;

  for(i=0;i<NUM_MOTORS;i++){

    // Set direction
    if(motors[i].idealSpeed > 0){
      biasIndex = BIAS_FORWARD;
      motors[i].motor->run(FORWARD);
    } else {
      biasIndex = BIAS_BACKWARD;
      motors[i].motor->run(BACKWARD);
    }
    
    // Apply bias and clip at speed limit
    motors[i].biasedSpeed = motors[i].idealSpeed + motors[i].bias[biasIndex];
    motors[i].biasedSpeed = constrain(motors[i].biasedSpeed,-SPEED_LIMIT,SPEED_LIMIT);
    
    // Set PWM values
    motors[i].motor->setSpeed((int) (abs(motors[i].biasedSpeed)*PWM_SPEED_FACTOR));

    Serial1.println((int) (abs(motors[i].biasedSpeed)*PWM_SPEED_FACTOR));
  }
 }

/* void updateBiases()
 *  Looks at where we actually are relative to where we though we'd be
 *  and adjusts the bias fields of each motor in an attempt to make
 *  future iterations more accurate.
 */
 void updateBiases(){
  int i, biasIndex;
  float actualSpeed;
  
  //Get "end of tick" position
  WheelPos curs;
  trackGetPos(&curs);
  motors[X1].endPos = curs.x1;
  motors[Y1].endPos = curs.y1;
  motors[X2].endPos = curs.x2;
  motors[Y2].endPos = curs.y2;

  for(i=0;i<NUM_MOTORS;i++){
    //Calculate real speed
    actualSpeed = motors[i].endPos - motors[i].startPos;
  
    //Only keep difference as bias
    if(motors[i].idealSpeed > 0)
      biasIndex = BIAS_FORWARD;
    else
      biasIndex = BIAS_BACKWARD;
    motors[i].bias[biasIndex] = motors[i].idealSpeed - actualSpeed;
  }
 }

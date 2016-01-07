/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->  http://www.adafruit.com/products/1438
*/

#include "tracking.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *x1 = AFMS.getMotor(1);
Adafruit_DCMotor *x2 = AFMS.getMotor(3);
Adafruit_DCMotor *y1 = AFMS.getMotor(2);
Adafruit_DCMotor *y2 = AFMS.getMotor(4);

// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

void setup() {
  trackInit();
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  // Set the speed to start, from 0 (off) to 255 (max speed)
}

void loop() {
  uint8_t i;
  Serial.print("Starting\n");
  x1->setSpeed(250);
  x2->setSpeed(250);
  y1->setSpeed(250);
  y2->setSpeed(250);
  x1->run(FORWARD);
  x2->run(FORWARD);
  y1->run(FORWARD);
  y2->run(FORWARD);
  delay(1);
  x1->setSpeed(0);
  x2->setSpeed(0);
  y1->setSpeed(0);
  y2->setSpeed(0);
  WheelPos currentPosition={0,0,0,0};
  WheelVel currentVelocity={0,0};
  trackGetPos(&currentPosition);
  Serial.print("x1 position: %n\n",currentPosition.x1);
  Serial.print("x2 position: %n\n",currentPosition.x2);
  Serial.print("y1 position: %n\n",currentPosition.y1);
  Serial.print("y2 position: %n\n",currentPosition.y2);
  Serial.print("xVel: %n\n",currentVelocity.x);
  Serial.print("yVel: %n\n",currentVelocity.y);
}

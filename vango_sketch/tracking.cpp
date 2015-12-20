/*
 * Wheel tracking module
 *
 * Offers an abstraction to wheel tracking.
 * Provides position and speed tracking making use of
 * the on-chip Timer/Counter units with input capture.
 * Tracking is entirely asynchronous to the main program flow.
 *
 * Revision history:
 *
 * 0.1) First stab at making this module formally.
 *      Currently supports 2 sensors via Timer Input Capture
 *      blocks. Exposed to main program via agreed structures
 *      defined in the tracking.h
 *
 *      Speed measurement is currently untested because there
 *      is not chassis or motor control and the timers overflow
 *      pretty quickly. After testing with chassis, need to pick
 *      a good clock prescaler to capture a practical range of
 *      speeds without common overflow.
 *      
 * 0.2) Added implementation for sensors which only use external
 *      interrupt pins (no timers or input capture mode). This is
 *      needed due to unavailability of input capture pins on the
 *      Arduino Mega. See WheelSensorIN struct for new functionality.
 *      
 */


/* Setup **************************************************************
 **********************************************************************/

#include "tracking.h"
#include "Arduino.h"

#define NUM_IR_IC 2  // Number of attached IR sensors via input capture
#define NUM_IR_IN 2  // Number of attached IR sensors via standard interrupts

#define X 0
#define Y 1


/* Internal structure to generalise IR sensors
 * attached via Timer/Counter Input Capture units
 */
typedef struct
{
  // Timer setup
  volatile uint8_t *cfg_reg;
  volatile uint8_t *int_reg;
  volatile uint16_t *tmr_reg;
  volatile unsigned int *cnt_reg;

  // Sensor state
  int dir;
  int count;
  int vel;
  int oflow;
} WheelSensorIC;

/* Internal structure for IR sensors connected via external
 * interrupt pins - no Timer units
 */
typedef struct
{
  // Interrupt setup
  int int_pin;

  // Sensor state
  int dir;
  int count;
} WheelSensorIN;

/* Initialisation for IR sensor structs
 */
WheelSensorIC ir_ic[NUM_IR_IC] = {
{
  .cfg_reg=&TCCR4B, // Timer 4 (input on D49)
  .int_reg=&TIMSK4,
  .tmr_reg=&TCNT4,
  .cnt_reg=&ICR4,
  .dir=1
},
{
  .cfg_reg=&TCCR5B, // Timer 5 (input on D48)
  .int_reg=&TIMSK5,
  .tmr_reg=&TCNT5,
  .cnt_reg=&ICR5,
  .dir=1
}
};

WheelSensorIN ir_in[NUM_IR_IN] = {
{
  .int_pin=2,
  .dir=1
},
{
  .int_pin=3,
  .dir=1
}
};

/* Required prototypes
 */
void int_sensor_tick_X();
void int_sensor_tick_Y();


/* Public functions ***************************************************
 **********************************************************************/

void trackInit(void){
  int i;

  // Initialise all IC sensors
  for(i=0;i<NUM_IR_IC;i++){
    *(ir_ic[i].cfg_reg) = _BV(CS12)  // Set clock divider to 1024
                     | _BV(CS10)
                     | _BV(ICNC1) // Enable noise canceller
                     | _BV(ICES1);// Enable input capture

    *(ir_ic[i].int_reg) = _BV(ICIE1) // Enable capture IRQ
                     | _BV(TOIE1);// Enable overflow IRQ
  }

  // Initialise all IN sensors
  pinMode(ir_in[0].int_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(ir_in[0].int_pin), int_sensor_tick_X, RISING);
  pinMode(ir_in[1].int_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(ir_in[1].int_pin), int_sensor_tick_Y, RISING);
}


void trackSetDir(int x, int y){
  ir_ic[X].dir=x;
  ir_in[X].dir=x;
  ir_ic[Y].dir=y;
  ir_in[Y].dir=y;

  return;
}


void trackGetPos(WheelPos *pos){
  pos->x1 = ir_ic[X].count;
  pos->x2 = ir_in[X].count;
  pos->y1 = ir_ic[Y].count;
  pos->y2 = ir_in[Y].count;
}


void trackGetVel(WheelVel *vel){
  vel->x = ir_ic[0].vel;
  vel->y = ir_ic[1].vel;
}



/* Private functions **************************************************
 **********************************************************************/

/* timer_capture(int i)
 *  Handle an event on sensor i
 */
inline void timer_capture(int i) {

  //Reset timer (not automatic with NORMAL mode)
  ir_ic[i].tmr_reg = 0;

  //Increment tick count
  ir_ic[i].count+=ir_ic[i].dir;

  //Find speed when no overflowed
  if(ir_ic[i].oflow)
    ir_ic[i].oflow=0;
  else
    ir_ic[i].vel=ir_ic[i].dir*(*(ir_ic[i].cnt_reg)>>1);
}


/* timer_overflow(int i)
 *  Handle overflow of capture timer on sensor i
 */
inline void timer_overflow(int i) {
  ir_ic[i].oflow=1;
  ir_ic[i].vel=0;//TODO add to speed on overflow
}

/* int_sensor_tick(int i)
 *  Handle tick of sensors which don't use timers
 */
inline void int_sensor_tick(int i){
  //Emulate 4 cycle latch of input capture
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  //TODO avoid overhead of digitalRead
  if(digitalRead(ir_in[i].int_pin))
    ir_in[i].count+=ir_in[i].dir;
}

// Link actual ISRs to the handler functions above
ISR(TIMER4_OVF_vect) {timer_overflow(X);}
ISR(TIMER5_OVF_vect) {timer_overflow(Y);}
ISR(TIMER4_CAPT_vect) {timer_capture(X);}
ISR(TIMER5_CAPT_vect) {timer_capture(Y);}
void int_sensor_tick_X(){int_sensor_tick(X);}
void int_sensor_tick_Y(){int_sensor_tick(Y);}

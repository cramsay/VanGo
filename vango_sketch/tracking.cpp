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
 */


/* Setup **************************************************************
 **********************************************************************/

#include "tracking.h"
#include "Arduino.h"

#define NUM_IR 2  // Number of attached IR sensors


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
} WheelSensor;


/* Initialisation for IR sensor structs
 */
WheelSensor ir[NUM_IR] = { 
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



/* Public functions ***************************************************
 **********************************************************************/

void trackInit(void){
  int i;

  // Initialise all sensors
  for(i=0;i<NUM_IR;i++){
    *(ir[i].cfg_reg) = _BV(CS11)  // Set clock divider to 1
                     | _BV(CS11)
                     | _BV(ICNC1) // Enable noise canceller
                     | _BV(ICES1);// Enable input capture
                     
    *(ir[i].int_reg) = _BV(ICIE1) // Enable capture IRQ
                     | _BV(TOIE1);// Enable overflow IRQ
  }
}


void trackSetDir(int x, int y){
  ir[0].dir=x;
  return;
}


void trackGetPos(WheelPos *pos){
  pos->x1 = ir[0].count;
  pos->y1 = ir[1].count;
}


void trackGetVel(WheelVel *vel){
  vel->x = ir[0].vel;
  vel->y = ir[1].vel;
}



/* Private functions **************************************************
 **********************************************************************/

/* timer_capture(int i)
 *  Handle an event on sensor i
 */
inline void timer_capture(int i) {
  
  //Reset timer (not automatic with NORMAL mode)
  ir[i].tmr_reg = 0;

  //Increment tick count
  ir[i].count+=ir[i].dir;

  //Find speed when no overflowed
  if(ir[i].oflow)
    ir[i].oflow=0;
  else
    ir[i].vel=ir[i].dir*(*(ir[i].cnt_reg)>>1);
}


/* timer_overflow(int i)
 *  Handle overflow of capture timer on sensor i
 */
inline void timer_overflow(int i) {
  ir[i].oflow=1;
  ir[i].vel=0;
}


// Link actual ISRs to the handler functions above
ISR(TIMER4_OVF_vect) {timer_overflow(0);}
ISR(TIMER5_OVF_vect) {timer_overflow(1);}
ISR(TIMER4_CAPT_vect) {timer_capture(0);}
ISR(TIMER5_CAPT_vect) {timer_capture(1);}

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
 * 0.3) Functionally the same as 0.2, but I've introduced some inheritance
 * 	to the structs for the IR sensors. The sensors connected via the
 * 	input capture units include all the functionality and state of
 * 	the interrupt driven sensors - with some added timer state and
 * 	config. How do we bodge inheritance in C? We ensure the memory layout
 * 	of the start of the struct of the "subclass" is the same as that
 * 	of the superclass. This is as easy as ensuring the first element
 * 	in the subclass is a superclass struct! Now we can cast upwards
 * 	from sub to super and treat input capture based sensors as standard
 * 	interrupt sensors when it suits us. Why not use c++ here instead?
 * 	...We're purists(?)
 *
 * 	Ideally we would define the entire configuration through these
 * 	structs to make modification easy, but this makes generating
 * 	the ISRs in a low overhead way a little tricky. Maybe something
 * 	for the future though.
 */


/* Setup **************************************************************
 **********************************************************************/

#include "tracking.h"
#include "Arduino.h"

#define NUM_IR 4  // Number of attached IR sensors
#define NUM_IR_IC 0  // Number of attached IR sensors via input capture

#define IS_X(i) ((i)%2)==0
#define IS_Y(i) !IS_X(i)

/* Internal structure for ALL IR sensors
 */
typedef struct
{
    // Interrupt setup
    int pin;
    // Sensor state
    int dir;
    int count;
} WheelSensor;

/* Extension of WheelSensor to handle input capture units
 */
typedef struct
{
    WheelSensor super;
    // Timer setup
    volatile uint8_t *cfg_reg;
    volatile uint8_t *int_reg;
    volatile uint16_t *tmr_reg;
    volatile unsigned int *cnt_reg;
    // Sensor state
    int period;
    int period_prev;
} WheelSensorIC;


/* Initialisation for IR sensor structs
 */
WheelSensor ir_y1 = {
    .pin=18,
    .dir=1,
};
WheelSensor ir_x1 = {
    .pin=19,
    .dir=1,
};
WheelSensor ir_x2 = {
    .pin=2,
    .dir=1,
};
WheelSensor ir_y2 = {
    .pin=3,
    .dir=1,
};

WheelSensor* all_ir[] = {&ir_x1, &ir_y1, &ir_x2, &ir_y2};
WheelSensorIC* ic_ir[] = {};  /* In final version, the motor control
                               *  doesn't actually use input capture speed
                               *  measurements so we have settled with using
                               *  only interrupt based approaches for sake of
                               *  consistency
                               */

/* Required prototypes
 */
void int_sensor_tick_X1();
void int_sensor_tick_Y1();
void int_sensor_tick_X2();
void int_sensor_tick_Y2();



/* Public functions ***************************************************
 **********************************************************************/

void trackInit(void) {
    int i;

    // Initialise all IC sensors
    for(i=0; i<NUM_IR_IC; i++) {
        *(ic_ir[i]->cfg_reg) = _BV(CS12)    // Set clock divider to 1024
                               | _BV(CS10)
                               | _BV(ICNC1) // Enable noise canceller
                               | _BV(ICES1);// Enable input capture

        *(ic_ir[i]->int_reg) = _BV(ICIE1)   // Enable capture IRQ
                               | _BV(TOIE1);// Enable overflow IRQ
    }


    // Initialise all IN sensors
    pinMode(all_ir[X1]->pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(all_ir[X1]->pin), int_sensor_tick_X1, RISING);
    pinMode(all_ir[Y1]->pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(all_ir[Y1]->pin), int_sensor_tick_Y1, RISING);
    pinMode(all_ir[X2]->pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(all_ir[X2]->pin), int_sensor_tick_X2, RISING);
    pinMode(all_ir[Y2]->pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(all_ir[Y2]->pin), int_sensor_tick_Y2, RISING);
}


void trackSetDir(int x1, int y1, int x2, int y2) {
    all_ir[X1]->dir=x1<0?-1:1;
    all_ir[Y1]->dir=y1<0?-1:1;
    all_ir[X2]->dir=x2<0?-1:1;
    all_ir[Y2]->dir=y2<0?-1:1;
}


void trackGetPos(WheelPos *pos) {
    pos->x1 = all_ir[X1]->count;
    pos->y1 = all_ir[Y1]->count;
    pos->x2 = all_ir[X2]->count;
    pos->y2 = all_ir[Y2]->count;
}


void trackGetVel(WheelVel *vel) {
    vel->x = 1.0 / (ic_ir[X1]->period_prev * ic_ir[X1]->super.dir);
    vel->y = 1.0 / (ic_ir[Y1]->period_prev * ic_ir[Y1]->super.dir);
}



/* Private functions **************************************************
 **********************************************************************/

/* timer_capture(int i)
 *  Handle an event on sensor i
 */
inline void timer_capture(int i) {

    //Save period
    ic_ir[i]->period_prev = ic_ir[i]->period + *(ic_ir[i]->cnt_reg);
    ic_ir[i]->period = 0;
    
    //Reset timer (not automatic with NORMAL mode)
    ic_ir[i]->tmr_reg = 0;

    //Increment tick count
    ic_ir[i]->super.count+=ic_ir[i]->super.dir;
}


/* timer_overflow(int i)
 *  Handle overflow of capture timer on sensor i
 */
inline void timer_overflow(int i) {
    ic_ir[i]->period+=0xFFFF; //TODO What should happen if dir changes during period?
}

/* int_sensor_tick(int i)
 *  Handle tick of sensors which don't use timers
 */
inline void int_sensor_tick(int i) {
    //Emulate 4 cycle latch of input capture
    __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

    //TODO avoid overhead of digitalRead
    if(digitalRead(all_ir[i]->pin))
        all_ir[i]->count+=all_ir[i]->dir;
}

// Link actual ISRs to the handler functions above
ISR(TIMER4_OVF_vect) {
    timer_overflow(X1);
}
ISR(TIMER5_OVF_vect) {
    timer_overflow(Y1);
}
ISR(TIMER4_CAPT_vect) {
    timer_capture(X1);
}
ISR(TIMER5_CAPT_vect) {
    timer_capture(Y1);
}
void int_sensor_tick_X1() {
    int_sensor_tick(X1);
}
void int_sensor_tick_Y1() {
    int_sensor_tick(Y1);
}
void int_sensor_tick_X2() {
    int_sensor_tick(X2);
}
void int_sensor_tick_Y2() {
    int_sensor_tick(Y2);
}

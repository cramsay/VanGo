/*
 * Communications  module
 *
 * Offers an abstraction to communication over Bluetooth
 * and packet format.
 * 
 * No Bluetooth specific code as this is done by a UART ->
 * Bluetooth hardware module. Packet format is designed specifically
 * make parsing on Arduino simple. Each packet is a line like so:
 * 	[x],[y],[penDown]
 *
 * Very basic flow control is implemented to ensure the hardware UART
 * buffers don't get overrun. We only ever send under the buffer
 * capacity at once. Upon the buffer running empty, we send a message
 * to the app to signal packets can be sent. With this we get a small
 * delay while the buffer refills, but this is OK for us.
 *
 */

/* Setup **************************************************************
 **********************************************************************/

#include "coms.h"
#include "tracking.h"
#include "Arduino.h"

#define isdigit(x) ((x) >= '0' && (x) <= '9')

/* Public functions ***************************************************
 **********************************************************************/

void comsInit(void) {
	Serial1.begin(115200);
  Serial.setTimeout(10000);
}

void comsGetNextInstr(WheelPos *pos, int *penDown) {

  //If no packets are buffered, ask for some and wait
	if(Serial1.available()==0)
		Serial1.println("Meer instructies, alsjeblieft");
	while(Serial1.available()==0)
		delay(50);

  //Parse packet
	pos->x1 = Serial1.parseInt();
	pos->y1 = Serial1.parseInt();
	*penDown = Serial1.parseInt();
	pos->x2 = pos->x1; 
	pos->y2 = pos->y1; 

  //Consume trailing chars
  while(!isdigit(Serial1.peek()) && Serial1.available()>0)
    Serial1.read();
}

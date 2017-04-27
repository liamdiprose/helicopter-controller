/*
 * quad_encoder.c
 *
 *  Created on: Apr 27, 2017
 *      Author: and22
 */

#include "quad_encoder.h"



//initialise the quadrature
void quad_init(void){
	yaw=0;
	pin_state=0;
}


void quad_measure(void){

	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0);
	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0);
	uint32_t pinA=GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_0);
	uint32_t pinB=GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_1);
	// bitshift left pinstate to move most recent read to 'old state', then combine pinA and pinB and or those with pin state, then and it with 15 to make sure only the first 4 bits are read
	pin_state = ((pin_state << 2) | ((pinA << 1) | pinB) & 15);
	yaw += quad_lookup[pin_state];
}

// give the current yaw (quadrature encoded) position in degrees
float quad_get_degrees(){
	return (360/112)*yaw;
}

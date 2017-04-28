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
	quad_lookup[0]=0;
	quad_lookup[1]=1;
	quad_lookup[2]=-1;
	quad_lookup[3]=0;
	quad_lookup[4]=-1;
	quad_lookup[5]=0;
	quad_lookup[6]=0;
	quad_lookup[7]=1;
	quad_lookup[8]=1;
	quad_lookup[9]=0;
	quad_lookup[10]=0;
	quad_lookup[11]=-1;
	quad_lookup[12]=0;
	quad_lookup[13]=1;
	quad_lookup[14]=-1;
	quad_lookup[15]=0;
}


void quad_measure(void){

	GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_0 |GPIO_PIN_1);
	qpins=GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_0 | GPIO_PIN_1) ;
	// bitshift left pinstate to move most recent read to 'old state', then combine pinA and pinB and or those with pin state, then and it with 15 to make sure only the first 4 bits are read
	pin_state = (((pin_state << 2) | qpins ) & 15);
	yaw += quad_lookup[pin_state];
	if (quad_lookup[pin_state]==0){
		yaw_delta++;
	}
}

// give the current yaw (quadrature encoded) position in degrees
int32_t quad_get_degrees(){
	return (360/224)*yaw;
}

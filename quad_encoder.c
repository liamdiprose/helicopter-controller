/*
 * quad_encoder.c
 *
 *  Created on: Apr 27, 2017
 *      Author: and22
 */

#include "quad_encoder.h"

//initialise the quadrature
void quad_init(void) {
	yaw = 0;
	pin_state = 0;
	quad_lookup[0] = 0;
	quad_lookup[1] = 1;
	quad_lookup[2] = -1;
	quad_lookup[3] = 0;
	quad_lookup[4] = -1;
	quad_lookup[5] = 0;
	quad_lookup[6] = 0;
	quad_lookup[7] = 1;
	quad_lookup[8] = 1;
	quad_lookup[9] = 0;
	quad_lookup[10] = 0;
	quad_lookup[11] =- 1;
	quad_lookup[12] = 0;
	quad_lookup[13] = 1;
	quad_lookup[14] = -1;
	quad_lookup[15] = 0;

	GPIOPinTypeGPIOInput(QUAD_GPIO_BASE, QUAD_GPIO_PINS);
	//Add the interrupt to the table
	GPIOIntRegister(QUAD_GPIO_BASE, quad_measure);
	// Set the interrupt to trigger on both edges of the pulse
	GPIOIntTypeSet(QUAD_GPIO_BASE, QUAD_GPIO_PINS, GPIO_BOTH_EDGES);
	GPIOIntEnable(QUAD_GPIO_BASE, QUAD_GPIO_PINS); 
}


void quad_measure (void) {

	GPIOIntClear(QUAD_GPIO_BASE, QUAD_GPIO_PINS);
	uint32_t current_inputs = GPIOPinRead(GPIO_PORTB_BASE, 
                    GPIO_PIN_0 | GPIO_PIN_1);
    
    // Construct state of quadrature encoder from combination of previous and 
    // current values of input channels.
    // Encoder state is 4 bits long, holding previous and current states of 
    // pins.
	encoder_state = ((encoder_state << 2) | qpins ) & 15;
    
    // Find if rotation is detected by consulting  lookup table with 
    // encoder state. Adjust yaw by 
	yaw += quad_lookup[encoder_state];
}

// give the current yaw (quadrature encoded) position in degrees
int32_t quad_get_degrees(){
	return 360 * yaw / 224;
}


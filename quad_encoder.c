/*
 * quad_encoder.c
 *
 *  Created on: Apr 27, 2017
 *      Author: and22
 */

#include "quad_encoder.h"

// Forward Declearation Table for ISR's
void quad_update_routine(void);

// Lookup table for quadrature encoder
int8_t quad_lookup[16];

// File globals
int32_t g_yaw;
uint8_t encoder_state;

// Initialise Quadrature Encoder
void quad_init(void) {
	g_yaw = 0;
	encoder_state = 0;
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

    SysCtlPeripheralEnable(QUAD_GPIO_PERIPH);
    while(!SysCtlPeripheralReady(QUAD_GPIO_PERIPH));

	GPIOPinTypeGPIOInput(QUAD_GPIO_BASE, QUAD_GPIO_PINS);
	//Add the interrupt to the table
	GPIOIntRegister(QUAD_GPIO_BASE, quad_update_routine);
	// Set the interrupt to trigger on both edges of the pulse
	GPIOIntTypeSet(QUAD_GPIO_BASE, QUAD_GPIO_PINS, GPIO_BOTH_EDGES);
	GPIOIntEnable(QUAD_GPIO_BASE, QUAD_GPIO_PINS); 
}

// Interrupt Routine. Runs on pin state change
void quad_update_routine (void) {

	GPIOIntClear(QUAD_GPIO_BASE, QUAD_GPIO_PINS);
	uint32_t current_inputs = GPIOPinRead(QUAD_GPIO_BASE, QUAD_GPIO_PINS);
    
    // Construct state of quadrature encoder from combination of previous and 
    // current values of input channels.
    // Encoder state is 4 bits long, holding previous and current states of 
    // pins.
	encoder_state = ((encoder_state << 2) | current_inputs ) & 15;
    
    // Find if rotation is detected by consulting  lookup table with 
    // encoder state. Adjust yaw by 
	g_yaw += quad_lookup[encoder_state];
}

// give the current yaw (quadrature encoded) position in degrees
int32_t quad_get_degrees(){
	return 360 * g_yaw / 224;
}


/*
 * quad_encoder.c
 *
 *  Created on: Apr 27, 2017
 *      Author: and22
 */

#include "quad_encoder.h"
#include "debug.h"
// Forward Declearation Table for ISR's
void quad_update_routine(void);

// Lookup table for quadrature encoder
int8_t quad_lookup[16];

// File globals
int32_t g_yaw = 0;
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

	//DEBUG("Quad update routine");
	GPIOIntClear(QUAD_GPIO_BASE, QUAD_GPIO_PINS);

	uint8_t input_a = GPIOPinRead(QUAD_GPIO_BASE, QUAD_GPIO_PIN_0) > 0;
	uint8_t input_b = GPIOPinRead(QUAD_GPIO_BASE, QUAD_GPIO_PIN_1) > 0;

    // Construct state of quadrature encoder from combination of previous and 
    // current values of input channels.
    // Encoder state is 4 bits long, holding previous and current states of 
    // pins.
	//DEBUG("a:%d b:%d", input_a, input_b);
	encoder_state = ((encoder_state << 2) | (input_a << 1) | (input_b << 0) ) & 15;
    
    // Find if rotation is detected by consulting  lookup table with 
    // encoder state. Adjust yaw by 

	int32_t delta = quad_lookup[encoder_state];
	//DEBUG("state: %d -> %d", encoder_state, delta);

	if (delta > 0) {
		//DEBUG("Adding 1!");
		///g_yaw++;
	} else if (delta < 0) {
		//DEBUG("Subtracting 1!);
		///g_yaw--;
	}


	g_yaw += quad_lookup[encoder_state];
}

// give the current yaw (quadrature encoded) position in degrees
int32_t quad_get_degrees(){
	return 360 * g_yaw / 224.0;
}


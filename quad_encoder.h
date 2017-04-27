/*
 * quad_encoder.h
 *
 *  Created on: Apr 27, 2017
 *      Author: and22
 */


#ifndef QUAD_ENCODER_H_
#define QUAD_ENCODER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


// Lookup table for quadrature encoder
int32_t quad_lookup[16] = {
		0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, 1, -1, 0
};

//yaw global
uint32_t yaw;

//pin state global
uint32_t pin_state;

//initialise the quadrature
void quad_init(void);

//interrupt to be registered
void quad_measure(void);

// give the current yaw (quadrature encoded) position in degrees
float quad_get_degrees();


#endif /* QUAD_ENCODER_H_ */



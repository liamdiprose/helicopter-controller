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
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

// Lookup table for quadrature encoder
int32_t quad_lookup[16];
uint32_t qpins;

int32_t num_rotations;
//= {
//0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, 1, -1, 0
//};

//yaw global
int32_t yaw;
int32_t yaw_delta; // for debug
//pin state global
int32_t pin_state;

//initialise the quadrature
void quad_init(void);

//interrupt to be registered
void quad_measure(void);

// give the current yaw (quadrature encoded) position in degrees
int32_t quad_get_degrees();


#endif /* QUAD_ENCODER_H_ */



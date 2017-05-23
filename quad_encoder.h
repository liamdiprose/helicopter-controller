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

#define QUAD_GPIO_BASE GPIO_PORTB_BASE
#define QUAD_GPIO_PIN_0 GPIO_PIN_0
#define QUAD_GPIO_PIN_1 GPIO_PIN_1
#define QUAD_GPIO_PINS (QUAD_GPIO_PIN_0 | QUAD_GPIO_PIN_1)

#define QUAD_GPIO_INT_PIN_0 GPIO_INT_PIN_0
#define QUAD_GPIO_INT_PIN_1 GPIO_INT_PIN_1
#define QUAD_GPIO_INT_PINS (QUAD_GPIO_INT_PIN_0 | QUAD_GPIO_INT_PIN_1)

// Lookup table for quadrature encoder
int8_t quad_lookup[16];

//yaw global
int32_t g_yaw;
uint8_t encoder_state;

//initialise the quadrature
void quad_init(void);

//interrupt to be registered
void quad_measure(void);

// give the current yaw (quadrature encoded) position in degrees
int32_t quad_get_degrees();


#endif /* QUAD_ENCODER_H_ */



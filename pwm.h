/*
 * pwm.h
 *
 *  Created on: Mar 22, 2017
 *      Author: ldi30
 */

#ifndef PWM_H_
#define PWM_H_

#include <stdio.h>

typedef struct pwmout_s {
	int frequency;
	float duty_cycle;
	uint32_t address;
} PWMOut;

// Create a PWM output handler
PWMOut pwm_init(uint32_t address, int frequency, int duty_cycle);

// Set the duty cycle of the pwm output
int pwm_duty_cycle_set(PWMOut pin, float duty_cycle);

// Set the frequncy of the pwm output
int pwm_frequency_set(PWMOut pin, int frequency);

#endif /* PWM_H_ */

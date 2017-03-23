/*
 * pwm.h
 *
 *  Created on: Mar 22, 2017
 *      Author: ldi30
 */

#ifndef PWM_H_
#define PWM_H_

#include <stdio.h>


#define DEFAULT_FREQUENCY 150
#define DEFAULT_DUTY_CYCLE 0.5

typedef struct pwmout_s {
	uint32_t base;
	uint32_t gen;
	uint32_t out;
	uint32_t frequency;
	float duty_cycle;
} PWMOut;

// Create a PWM output handler
PWMOut pwm_init(uint32_t addr_base, uint32_t clk_gen, uint32_t out, uint32_t outbit);

// Set the duty cycle of the pwm output
int pwm_duty_cycle_set(PWMOut pin, float duty_cycle);

// Set the frequncy of the pwm output
int pwm_frequency_set(PWMOut pin, int frequency);

#endif /* PWM_H_ */

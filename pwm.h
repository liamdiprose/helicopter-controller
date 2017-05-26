#ifndef PWM_H_
#define PWM_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"

#define PWM_DEFAULT_FREQUENCY 150
#define PWM_DEFAULT_DUTY_CYCLE 0.5

#define PWM_STATE_ON true
#define PWM_STATE_OFF false

typedef struct pwmout_s {
	uint32_t base;
	uint32_t gen;
	uint32_t out;
	uint32_t outbit;
	uint32_t period;
	uint8_t duty_cycle;
} PWMOut;

// Create a PWM output handler
PWMOut pwm_init(uint32_t pwm_periph, uint32_t addr_base, uint32_t clk_gen, uint32_t out, uint32_t outbit);

void pwm_init_gpio(PWMOut* pwm_out, uint32_t periph, uint32_t port, uint32_t pin, uint32_t pin_conf);

// Turn the PWM output on
void pwm_set_state(PWMOut pin, bool new_state);

// Set the duty cycle of the pwm output
void pwm_duty_cycle_set(PWMOut* pin, uint8_t duty_cycle);

// Set the frequncy of the pwm output
void pwm_frequency_set(PWMOut* pin, uint32_t frequency);

#endif /* PWM_H_ */

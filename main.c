/*
 * main.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define PART_TM4C123GH6PM
#include "driverlib/port_map.h"

#define ALT_BUTTON_STEP_MUTLIPLER 0.05
#define YAW_BUTTON_STEP_MULTIPLIER 0.15

// Interrupts
// User inputs
// Quadrature Signals


int main(void) {
	
	// Initialise Variables
	PIDConfig pid_alt = {.P = 1, .I = 0, .D = 0 };
	PIDConifg pid_yaw = {.P = 1, .I = 0, .D = 0 };

	PWMOut alt_output = pwm_init(PWM_BASE, PWM_GEN_3, PWM_OUT_7, 150, 0.5);
	PWMOut yaw_output = pwm_init(PWM_BASE, PWM_GEN_3, PWM_OUT_6, 150, 0.5);

	Button button_alt_up = button_init(GPIO_BASE, GPIO_PIN);
	Button button_alt_down = button_init(GPIO_BASE, GPIO_PIN);

	// Calibration

	while (1) {
		// Copy current height from ADC
		current_alt = 0; //TODO
		// Calculate rotational position from Quadrature interrupt inputs 
		current_yaw = 0;

		// Milestone 1 Exculsive
		// If frequency on PWM control input changes, update the PWM outputs
		if (/*interrupt on frequncy control pin*/0) {
			int new_freq = 0;  // STUB
			pwm_frequency_set(alt_output, new_freq);
			pwm_frequency_set(yaw_output, new_freq);
		}
		
		// Update target Altitude and Yaw if respective button was pressed
		if (button_pressed(button_alt_up, current_time)) {
			pid_target_set(pid_alt, pid_alt.target * (1 + ALT_BUTTON_STEP_MUTLIPLER));
		}
		if (button_pressed(button_alt_down, current_time)) {
			pid_target_set(pid_alt, pid_alt.target * (1 - ALT_BUTTON_STEP_MUTLIPLER));
		}
		if (button_pressed(button_yaw_up, current_time)) {
			pid_target_set(pid_yaw, pid_yaw.target * (1 + YAW_BUTTON_STEP_MULTIPLER));
		}
		if (button_pressed(button_yaw_down, current_time)) {
			pid_target_set(pid_yaw, pid_yaw.target * (1 - YAW_BUTTON_STEP_MUTLIPLER);
		}
			
		// Calcluate the new duty cycle and send to PWM output.
		current_alt_dc = pid_update(pid_alt, current_alt);
		pwm_duty_cycle_set(alt_output, current_alt_dc);

		current_yaw_dc = pid_update(pid_yaw, current_yaw);
		pwm_duty_cycle_set(yaw_output, current_yaw_dc);

		// Update Display (skip updating every X ticks instead)
		display_print(string);

		// Send status to UART (skip updating every X ticks)
		uart_print("CSV data");
	}

	return 0;
}


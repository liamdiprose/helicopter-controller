/*
 * main.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"


#include "button.h"
#include "pwm.h"
#include "pid.h"


//#define PART_TM4C123GH6PM


#define ALT_BUTTON_STEP_MULTIPLIER 0.05
#define YAW_BUTTON_STEP_MULTIPLIER 0.15

#define ALT_INCREMENT(alt) (alt * (1 + ALT_BUTTON_STEP_MULTIPLIER))
#define ALT_DECREMENT(alt) (alt * (1 - ALT_BUTTON_STEP_MULTIPLIER))
#define YAW_INCREMENT(yaw) (yaw * (1 + YAW_BUTTON_STEP_MULTIPLIER))
#define YAW_DECREMENT(yaw) (yaw * (1 - YAW_BUTTON_STEP_MULTIPLIER))

// Interrupts
// User inputs
// Quadrature Signals




int main(void) {
	
	SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
	                   SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(4);
	PIDConfig pid_alt = {.P = 1, .I = 0, .D = 0 };
	PIDConfig pid_yaw = {.P = 1, .I = 0, .D = 0 };

	// Enable GPIO Input and Output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);   // PWM Main Rotor
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // BTN1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // BTN2
	Button button_alt_up = button_init(GPIO_PORTD_BASE, GPIO_PIN_2);
	Button button_alt_down = button_init(GPIO_PORTF_BASE, GPIO_PIN_0);

	// Enable PWM Module to generate PWM outputs
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

	PWMOut alt_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_7, PWM_OUT_7_BIT);
	// Set PC5 output to be PWM
	GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_5);
	// Link the connector to the PWM generator
	GPIOPinConfigure(GPIO_PC5_M0PWM7);


	//PWMOut yaw_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, 150, 0.5);

	// Declear variables that hold sensor input
	uint32_t current_alt = 0; //TODO
	uint32_t current_yaw = 0;

	// Declear variables that hold calculated duty cycles
	uint32_t new_alt_dc;
	uint32_t new_yaw_dc;


	while (1) {
		// Copy current height from ADC
		// Calculate rotational position from Quadrature interrupt inputs 


		// Milestone 1 Exculsive
		// If frequency on PWM control input changes, update the PWM outputs
/*
		if (*interrupt on frequncy control pin*0) {
			int new_freq = 0;  // STUB
			pwm_frequency_set(alt_output, new_freq);
			pwm_frequency_set(yaw_output, new_freq);
		}
*/
		// Update target Altitude and Yaw if respective button was pressed
		if (button_pressed(button_alt_up)) {
			pid_target_set(&pid_alt, ALT_INCREMENT(pid_alt.target));
		}
		if (button_pressed(button_alt_down)) {
			pid_target_set(&pid_alt, ALT_DECREMENT(pid_alt.target));
		}
		/*
		if (button_pressed(button_yaw_up, current_time)) {
			pid_target_set(&pid_yaw, YAW_INCREMENT(pid_yaw.target));
		}
		if (button_pressed(button_yaw_down, current_time)) {
			pid_target_set(&pid_yaw, YAW_DECREMENT(pid_yaw.target));
		}
	*/
		// Calcluate the new duty cycle and send to PWM output.
		new_alt_dc = pid_update(pid_alt, current_alt);
		pwm_duty_cycle_set(&alt_output, new_alt_dc);
/*
		new_yaw_dc = pid_update(pid_yaw, current_yaw);
		pwm_duty_cycle_set(&yaw_output, new_yaw_dc);
*/
		// Update Display (skip updating every X ticks instead)
		//display_print(string);

		// Send status to UART (skip updating every X ticks)
		//uart_csv(pid_alt, pid_yaw, pwm_alt, pwm_yaw);
	}
	return 0;
}

/*
 * main.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"


#include "button.h"
#include "pwm.h"
#include "pid.h"


//#define PART_TM4C123GH6PM


#define ALT_BUTTON_STEP_MULTIPLIER 0.05
#define YAW_BUTTON_STEP_MULTIPLIER 0.15

#define ALT_INCREMENT(alt)  (alt + ALT_BUTTON_STEP_MULTIPLIER) //(alt * (1 + ALT_BUTTON_STEP_MULTIPLIER))
#define ALT_DECREMENT(alt) (alt - ALT_BUTTON_STEP_MULTIPLIER) //* (1 - ALT_BUTTON_STEP_MULTIPLIER))
//#define YAW_INCREMENT(yaw) (yaw * (1 + YAW_BUTTON_STEP_MULTIPLIER))
//#define YAW_DECREMENT(yaw) (yaw * (1 - YAW_BUTTON_STEP_MULTIPLIER))

// Interrupts
// User inputs
// Quadrature Signals
Button button_alt_down;
Button button_alt_up;


void on_btn_up_press(void) {
	GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_2);
	uint32_t pins = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2);
	if (pins >> 2 & 1 == 1) {
		button_alt_up.last_pressed = TimerValueGet(TIMER5_BASE, TIMER_BOTH);
	}
}
void on_btn_down_press(void) {
	GPIOIntClear(GPIO_PORTE_BASE, GPIO_INT_PIN_0);
	uint32_t pins = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0);
	if (pins & 1 == 1) {
		button_alt_down.last_pressed = TimerValueGet(TIMER5_BASE, TIMER_BOTH);

	}
}

int main(void) {
	
	SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
	                   SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(4);
	PIDConfig pid_alt = {.P = 1, .I = 0, .D = 0 };
	PIDConfig pid_yaw = {.P = 1, .I = 0, .D = 0 };

	// Enable GPIO Input and Output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);   // PWM Main Rotor
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // BTN1 on PIN2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // BTN2 on PIN0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // TivaBoard Buttons
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);


	// Enable PWM Module to generate PWM outputs
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);


	PWMOut alt_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_7, PWM_OUT_7_BIT);
	// Set PC5 output to be PWM
	GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_5);
	// Link the connector to the PWM generator
	GPIOPinConfigure(GPIO_PC5_M0PWM7);
	button_alt_down = button_init(GPIO_PORTD_BASE, GPIO_PIN_2);
	button_alt_up = button_init(GPIO_PORTE_BASE, GPIO_PIN_0);



	GPIOIntRegister(GPIO_PORTD_BASE, on_btn_up_press);
	GPIOIntRegister(GPIO_PORTE_BASE, on_btn_down_press);
	GPIOIntTypeSet(GPIO_PORTD_BASE, 1 << 2, GPIO_FALLING_EDGE);
	GPIOIntTypeSet(GPIO_PORTE_BASE, 1 << 0, GPIO_FALLING_EDGE);
	GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_2);
	GPIOIntEnable(GPIO_PORTE_BASE, GPIO_INT_PIN_0);

	// Enable Timer for getting time inbetween button presses
	TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC_UP);
	TimerEnable(TIMER5_BASE, TIMER_BOTH);


	//PWMOut yaw_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, 150, 0.5);

	// Declear variables that hold sensor input
	uint32_t current_alt = 0; //TODO
	uint32_t current_yaw = 0;

	// Declear variables that hold calculated duty cycles
	float new_alt_dc = 0;
	float old_alt_dc = 0;
	uint32_t new_yaw_dc = 0;
/*
	while (1) {
		current_yaw = TimerValueGet(TIMER5_BASE, TIMER_BOTH);
	}
*/

	pid_target_set(&pid_alt, 0.50);
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
		if (button_pressed(&button_alt_up)) {
			pid_target_set(&pid_alt, pid_alt.target + 0.15);
		}
		if (button_pressed(&button_alt_down)) {
			pid_target_set(&pid_alt, pid_alt.target - 0.15);
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
		old_alt_dc = new_alt_dc;
		new_alt_dc = pid_update(pid_alt, current_alt);
		if (new_alt_dc != old_alt_dc) {
			pwm_duty_cycle_set(&alt_output, new_alt_dc);
		}
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



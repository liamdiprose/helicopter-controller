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
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"

#include "driverlib/pin_map.h"
#include "button.h"
#include "pwm.h"
#include "pid.h"
#include "pwmcounter.h"
#include "quad_encoder.h"
#include "AltitudeADC.h"

#define PWM_INPUT_THRESHOLD 10


//#define PART_TM4C123GH6PM


#define ALT_BUTTON_STEP_MULTIPLIER 0.05
#define YAW_BUTTON_STEP_MULTIPLIER 0.15

#define ALT_INCREMENT(alt)  (alt + ALT_BUTTON_STEP_MULTIPLIER) //(alt * (1 + ALT_BUTTON_STEP_MULTIPLIER))
#define ALT_DECREMENT(alt) (alt - ALT_BUTTON_STEP_MULTIPLIER) //* (1 - ALT_BUTTON_STEP_MULTIPLIER))

#define PWM_COUNTER_START 1000

//#define YAW_INCREMENT(yaw) (yaw * (1 + YAW_BUTTON_STEP_MULTIPLIER))
//#define YAW_DECREMENT(yaw) (yaw * (1 - YAW_BUTTON_STEP_MULTIPLIER))

// Interrupts
// User inputs
// Quadrature Signals



int main(void) {
	
	SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
	                   SYSCTL_XTAL_16MHZ);
	uint32_t clock = SysCtlClockGet();
	SysCtlPWMClockSet(SYSCTL_PWMDIV_8);
	PIDConfig pid_alt = {.P = 1, .I = 0, .D = 0 };
	//PIDConfig pid_yaw = {.P = 1, .I = 0, .D = 0 };

	// Enable GPIO Input and Output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // quadrature encoder
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);   // PWM Main Rotor
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // BTN1 on PIN2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // BTN2 on PIN0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);


	// Enable PWM Module to generate PWM outputs
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0));
		SysCtlDelay(9);  // Test: wait for

	PWMOut alt_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_7, PWM_OUT_7_BIT);
	// Set PC5 output to be PWM
	GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_5);
	// Link the connector to the PWM generator
	GPIOPinConfigure(GPIO_PC5_M0PWM7);
	// link the connector to the ADC input
	//GPIOPinConfigure(GPIO_PE4_M0AIN9);

	// Initialise buttons into global array
	buttons[BUTTON_ALT_UP] = button_init(GPIO_PORTE_BASE, GPIO_PIN_0);
	buttons[BUTTON_ALT_DOWN] = button_init(GPIO_PORTD_BASE, GPIO_PIN_2);

	// Initialise timer for checking buttons
	SysTickPeriodSet(SysCtlClockGet()/100);
	SysTickEnable();
	SysTickIntEnable();
	SysTickIntRegister(button_check);
	SysTickIntRegister(update_Altitude);


	// Enables for the quadrature encoding
	//
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_0);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_1);
	//Add the interrupt to the table
	GPIOIntRegister(GPIO_PORTB_BASE, quad_measure);
	// Set the interrupt to trigger on both edges
	GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1, GPIO_BOTH_EDGES);
	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
	// Initialises values, in the quad_encoder file

	quad_init();
	Altitude_init();

	pwmcounter_init();


	char oled_freq[24] = "yaw :\0";
	char oled_dc[24] = "dc:\0";

	//PWMOut yaw_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, 150, 0.5);

	uart_init();

	// Declear variables that hold sensor input
	uint32_t current_alt = 0;


	uint32_t freq = 1;

	uint32_t last_accepted_freq = 0;
	uint32_t duty_cycle = 0;

	while (1) {
		// Copy current height from ADC
		// Calculate rotational position from Quadrature interrupt inputs 

		//this should loop properly now, making sure that it stays within a valid range for the majority of the time.
		// actually fix this, its still broken
		if (yaw > 224) {
			yaw = yaw - 224;
			num_rotations += 1;
		}
		if (yaw < 0) {
			yaw = yaw + 224;
			num_rotations -= 1;
		}
		// Update target Altitude and Yaw if respective button was pressed
		if (button_read(&buttons[BUTTON_ALT_UP])) {
			pid_target_set(&pid_alt, pid_alt.target - 0.15);
		}
		if (button_read(&buttons[BUTTON_ALT_DOWN])) {
			pid_target_set(&pid_alt, pid_alt.target - 0.15);
		}

		if (button_read(&buttons[BUTTON_YAW_UP])) {
			pid_target_set(&pid_yaw, pid_yaw.target + 1);
		}

		if (button_read(&buttons[BUTTON_YAW_DOWN])) {
			pid_target_set(&pid_yaw, pid_yaw.target - 1);
		}


		display_update_alt(5);  // STUB
		display_update_yaw(5);  // STUB
	}
}

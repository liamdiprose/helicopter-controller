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


#define ALT_BUTTON_STEP_MULTIPLIER 0.05
#define YAW_BUTTON_STEP_MULTIPLIER 0.15

#define PWM_COUNTER_START 1000


int main(void) {
	
	SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
	                   SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

	// Enable GPIO Input and Output
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // quadrature encoder
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);   // PWM Main Rotor
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // BTN1 on PIN2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // BTN2 on PIN0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);

	PIDConfig pid_alt = pid_init(1, 0, 0.01);
    PIDConfig pid_yaw = pid_init(1, 0, 0.01);
    
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
	// Initialises values, in the quad_encoder file

	quad_init();
	Altitude_init();

	PWMOut yaw_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, 150, 0.5);
	PWMOut alt_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, 150, 0.5);

	uart_init();

	// Declear variables that hold sensor input
	uint32_t current_alt = 0;



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

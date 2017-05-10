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
#include "OrbitOLEDInterface.h"
#include "utils/ustdlib.h"  // usprintf()
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



int freq_cap(uint32_t new_freq, uint32_t old_freq) {
	if (new_freq == 0) {
		return 150;
	} else if (new_freq < 100) {
		return 150;
	} else if (new_freq > 300) {
		return 150;
	} else {
		return new_freq;
	}
}

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

	// initialise timer for checking buttons
	SysTickPeriodSet(SysCtlClockGet()/100);
	SysTickEnable();
	SysTickIntEnable();
	SysTickIntRegister(button_check);
	SysTickIntRegister(update_Altitude);

	// Enable Timer for getting time inbetween button presses
	TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC_UP);
	TimerEnable(TIMER5_BASE, TIMER_BOTH);


	// Enable Pin to connect to timer
	GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_2);
	//GPIOPinConfigure(GPIO_PB0_T2CCP0); // correct pint
	GPIOPinConfigure(GPIO_PB2_T3CCP0); // liams broken pin workaround


	//enables for the quadrature encoding
	//
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_0);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_1);
	//Add the interrupt to the table
	GPIOIntRegister(GPIO_PORTB_BASE, quad_measure);
	// Set the interrupt to trigger on both edges
	GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1, GPIO_BOTH_EDGES);
	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);
	// initialises values, in the quad_encoder file
	quad_init();
	Altitude_init();


	pwmcounter_init();

	OLEDInitialise();
	char oled_freq[24] = "yaw :\0";
	char oled_dc[24] = "dc:\0";

	//PWMOut yaw_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, 150, 0.5);

	//uart_init();

	// Declear variables that hold sensor input
	uint32_t current_alt = 0;

	// Declear variables that hold calculated duty cycles
	float new_alt_dc = 0;
	float old_alt_dc = 0;


	uint32_t freq = 1;

	uint32_t last_accepted_freq = 0;
	uint32_t duty_cycle = 0;

	pid_target_set(&pid_alt, 50);

	//uart_print("TEST");

	while (1) {
		// Copy current height from ADC
		// Calculate rotational position from Quadrature interrupt inputs 

		// Test Timer is working
		//uint32_t time = TimerValueGet(TIMER3_BASE, TIMER_B);
//		uint32_t pwm = TimerValueGet(TIMER2_BASE, TIMER_A);

		//saved_pwm_pulses = freq; //TODO
		freq = freq_cap(pwmcounter_freq_get(), freq);



		if (abs(last_accepted_freq - freq) > 15 ) {
			// PWM input has changed, update to new frequency
			pwm_frequency_set(&alt_output, freq);
			last_accepted_freq = freq;
		}
		//this should loop properly now, making sure that it stays within a valid range for the majority of the time.
		// actually fix this, its still broken
		if (yaw > 224) {
			yaw = yaw-224;
			num_rotations +=1;
		}
		if (yaw < 0) {
			yaw =yaw+224;
			num_rotations -=1;
		}
		// Update target Altitude and Yaw if respective button was pressed
		if (button_read(&buttons[BUTTON_ALT_UP])) {
			//pid_target_set(&pid_alt, pid_alt.target + 0.15);
			if (alt_output.duty_cycle < 95) {
				pwm_duty_cycle_set(&alt_output, alt_output.duty_cycle + 5);
			}
		}
		if (button_read(&buttons[BUTTON_ALT_DOWN])) {
			//pid_target_set(&pid_alt, pid_alt.target - 0.15);
			if (alt_output.duty_cycle > 5) {
				pwm_duty_cycle_set(&alt_output, alt_output.duty_cycle - 5);
			}
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
		usprintf(oled_freq, "yaw:%d      ",quad_get_degrees());
		duty_cycle = alt_output.duty_cycle;
		//usprintf(oled_dc, "pin: %d%d%d%d",(pin_state)&1,(pin_state>>1)&1,(pin_state>>2)&1,(pin_state>>3)&1);
		usprintf(oled_dc, "Alt%%:%d   #rot:%d  ",altitude_get_percent(),num_rotations);
		OLEDStringDraw(oled_freq, 0, 0);
		OLEDStringDraw(oled_dc, 0, 1);

		// Send status to UART (skip updating every X ticks)
		//uart_csv(pid_alt, pid_yaw, pwm_alt, pwm_yaw);

	}
}



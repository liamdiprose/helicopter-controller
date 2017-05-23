#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"

#include "button.h"

#include "pwm.h"
#include "pid.h"
#include "quad_encoder.h"
#include "AltitudeADC.h"
#include "uart.h"
#include "display.h"

#define DEBUGGING_MODE 0
#define DEBUG(...) do {} while (0)
//#include "debug.h"


// Define the places of each button used
#define BUTTON_ALT_UP 0
#define BUTTON_ALT_DOWN 1
#define BUTTON_YAW_UP 2
#define BUTTON_YAW_DOWN 3

int main(void) {
	
	SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // quadrature encoder
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); // PWM Main Rotor
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // BTN1 on PIN2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // BTN2 on PIN0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);

	// Enable PWM Module to generate PWM outputs
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

	while (!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0));
	SysCtlDelay(9);  

	// Initialise buttons into global array
	buttons[BUTTON_ALT_UP] = button_init(GPIO_PORTE_BASE, GPIO_PIN_0);
	buttons[BUTTON_ALT_DOWN] = button_init(GPIO_PORTD_BASE, GPIO_PIN_2);
    //buttons[BUTTON_YAW_UP] = button_init(GPIO_PORT);
    //buttons[BUTTON_YAW_DOWN] = button_init(GPIO_PORT);

	SysTickIntRegister(update_Altitude);
	//uart_init();
   // DEBUG("UART Initialised");

	Altitude_init();
    DEBUG("ADC Initialised");
	quad_init();
    DEBUG("Quadrature Encoder Initialised");

	PIDConfig pid_alt = pid_init(1, 0, 0.01);
    PIDConfig pid_yaw = pid_init(1, 0, 0.01);
    DEBUG("PID Controllers Initialised");

    //PWMOut pwm_init(uint32_t addr_base, uint32_t clk_gen, uint32_t out, uint32_t outbit) {

	PWMOut alt_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, PWM_DEFAULT_FREQUENCY );
	//PWMOut yaw_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, PWM_DEFAULT_FREQUENCY, 0);
    DEBUG("PWM outputs Initialised");

	// Declear variables that hold sensor input
	uint32_t current_alt = 0;
	uint32_t current_yaw = 0;

    uint32_t previous_alt = 1;

    DEBUG("Starting main loop");
	while (1) {
		// Copy current height from ADC
        current_alt = altitude_current;
        current_yaw = g_yaw;

        // Ask each button if they have been pressed, and if so,
        // perform specific action assigned to them.
		if (button_read(&buttons[BUTTON_ALT_UP])) {
                // Altitude Up button Process
                DEBUG("Altitude up button pressed. New Altitude: %d"/*, pid_alt->target*/);
			  //  pid_target_set(&alt_output, pid_alt.target + ALT_INCREMENT);

		} else if (button_read(&buttons[BUTTON_ALT_DOWN])) {
                DEBUG("Altitude down button pressed. New Altitude: %d"/*, pid_alt->target*/);
			   // pid_target_set(&pid_alt, pid_alt.target - ALT_DECREMENT);
		}

		if (button_read(&buttons[BUTTON_YAW_UP])) {
                DEBUG("Yaw up button pressed. New Yaw: %d"/*, pid_yaw->target*/);
			    //pid_target_set(&pid_yaw, pid_yaw.target + YAW_INCREMENT);
		} else if (button_read(&buttons[BUTTON_YAW_DOWN])) {
                DEBUG("Yaw down button pressed. New Yaw: %d"/*, pid_yaw->target*/);
			    //pid_target_set(&pid_yaw, pid_yaw.target - YAW_DECREMENT);
		}

        pwm_duty_cycle_set(&alt_output, pid_update(&pid_alt, 0, current_alt));
        //pwm_duty_cycle_set(&yaw_output, pid_update(&yaw_alt, current_yaw));

        if (current_alt != previous_alt) {
		    display_update_alt(current_alt);
        }

        // UART?
	}
}

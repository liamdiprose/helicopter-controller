#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "inc/hw_memmap.h"

#include "button.h"
#include "pwm.h"
#include "pid.h"
#include "quad_encoder.h"
#include "adc.h"
#include "uart.h"
#include "display.h"
#include "timer.h"
#include "debug.h"

// Increase Altitude Button
#define BUTTON_ALT_UP_PERIPH SYSCTL_PERIPH_GPIOE
#define BUTTON_ALT_UP_BASE GPIO_PORTE_BASE
#define BUTTON_ALT_UP_PIN GPIO_PIN_0

// Decrease Altitude Button
#define BUTTON_ALT_DOWN_PERIPH SYSCTL_PERIPH_GPIOD
#define BUTTON_ALT_DOWN_BASE GPIO_PORTD_BASE
#define BUTTON_ALT_DOWN_PIN GPIO_PIN_2

// Rotate Clockwise Button
#define BUTTON_YAW_LEFT_PERIPH SYSCTL_PERIPH_GPIOF
#define BUTTON_YAW_LEFT_BASE GPIO_PORTF_BASE
#define BUTTON_YAW_LEFT_PIN GPIO_PIN_0

// Rotate Counter-clockwise Button
#define BUTTON_YAW_RIGHT_PERIPH SYSCTL_PERIPH_GPIOF
#define BUTTON_YAW_RIGHT_BASE GPIO_PORTF_BASE
#define BUTTON_YAW_RIGHT_PIN GPIO_PIN_4

// Flight Mode Switch
#define SWITCH_FMODE_PERIPH SYSCTL_PERIPH_GPIOA
#define SWITCH_FMODE_BASE GPIO_PORTA_BASE
#define SWITCH_FMODE_PIN GPIO_PIN_7

// Main Roter PWM Output
#define PWM_ALT_PWM_PERIPH SYSCTL_PERIPH_PWM0
#define PWM_ALT_PWM_BASE PWM0_BASE
#define PWM_ALT_GEN PWM_GEN_3
#define PWM_ALT_OUT PWM_OUT_7
#define PWM_ALT_OUTBIT PWM_OUT_7_BIT
#define PWM_ALT_GPIO_PERIPH SYSCTL_PERIPH_GPIOC
#define PWM_ALT_GPIO_BASE GPIO_PORTC_BASE
#define PWM_ALT_GPIO_PIN GPIO_PIN_5
#define PWM_ALT_GPIO_PIN_CONF GPIO_PC5_M0PWM7

// Tail Rotor PWM Output
#define PWM_YAW_PWM_PERIPH SYSCTL_PERIPH_PWM1
#define PWM_YAW_PWM_BASE PWM1_BASE
#define PWM_YAW_GEN PWM_GEN_2
#define PWM_YAW_OUT PWM_OUT_5
#define PWM_YAW_OUTBIT PWM_OUT_5_BIT
#define PWM_YAW_GPIO_PERIPH SYSCTL_PERIPH_GPIOF
#define PWM_YAW_GPIO_BASE GPIO_PORTF_BASE
#define PWM_YAW_GPIO_PIN GPIO_PIN_1
#define PWM_YAW_GPIO_PIN_CONF GPIO_PF1_M1PWM5


// Define possible states of program overall FSM
typedef enum {
	CALIBRATION, LANDED, FLYING, LANDING
} ProgramState;


void clock_init(void) {
	SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}

void all_the_routines(void) {
	 button_check_routine();
	 adc_update_routine();
}

int main(void) {
	
    clock_init();

	SysTickPeriodSet(SysCtlClockGet()/100);

    // Start timer
    timer_init();

    // Initiase UART
	uart_init();
    DEBUG("UART Initialised");

	// Initialise buttons
	Button* button_alt_up = button_init(BUTTON_ALT_UP_PERIPH, BUTTON_ALT_UP_BASE,
                    BUTTON_ALT_UP_PIN, PULL_UP);
	Button* button_alt_down = button_init(BUTTON_ALT_DOWN_PERIPH, BUTTON_ALT_DOWN_BASE,
                    BUTTON_ALT_DOWN_PIN, PULL_UP);
	Button* button_yaw_left = button_init(BUTTON_YAW_LEFT_PERIPH, BUTTON_YAW_LEFT_BASE,
                    BUTTON_YAW_LEFT_PIN, PULL_DOWN);
	Button* button_yaw_right = button_init(BUTTON_YAW_RIGHT_PERIPH, BUTTON_YAW_RIGHT_BASE,
                    BUTTON_YAW_RIGHT_PIN, PULL_DOWN);
	Button* button_flight_mode = button_init(SWITCH_FMODE_PERIPH, SWITCH_FMODE_BASE,
	    				SWITCH_FMODE_PIN, NONE);
	DEBUG("Buttons Initialised");

	// Start ADC module to read altitude
	adc_init();
    DEBUG("ADC Initialised");

    // Start quadrature encoder to read yaw
	quad_init();
    DEBUG("Quadrature Encoder Initialised");

    // Create PID controllers
	PIDConfig pid_alt = pid_init(1, 0, 0.01);
    PIDConfig pid_yaw = pid_init(1, 0, 0.01);
    DEBUG("PID Controllers Initialised");

    // Create PWM outputs to control rotors
	PWMOut alt_output = pwm_init(PWM_ALT_PWM_PERIPH, PWM_ALT_PWM_BASE, PWM_ALT_GEN, PWM_ALT_OUT, PWM_ALT_OUTBIT);
    pwm_init_gpio(&alt_output, PWM_ALT_GPIO_PERIPH, PWM_ALT_GPIO_BASE, PWM_ALT_GPIO_PIN, PWM_ALT_GPIO_PIN_CONF);

	PWMOut yaw_output = pwm_init(PWM_YAW_PWM_PERIPH, PWM_YAW_PWM_BASE, PWM_YAW_GEN, PWM_YAW_OUT, PWM_YAW_OUTBIT);
    pwm_init_gpio(&yaw_output, PWM_YAW_GPIO_PERIPH, PWM_YAW_GPIO_BASE, PWM_YAW_GPIO_PIN, PWM_YAW_GPIO_PIN_CONF);
    DEBUG("PWM outputs Initialised");

    // Start display to display TODO information
    display_init();
    DEBUG("Display Initialised");

    // Initialise Variable to track state of program FSM
    ProgramState program_state = LANDED;

    // Save events of flight mode switch
    //ButtonStatus flight_mode;

    // Track if program has calibrated yet
    bool calibrated = false;

	// Declear variables that hold sensor input
	uint32_t current_alt;
	uint32_t current_yaw;

	// Save variables to detect changes inbetween loops
    uint32_t previous_alt = 1;


    SysTickIntRegister(all_the_routines);
	SysTickEnable();
	SysTickIntEnable();


    DEBUG("Startup completed in %d ms", timer_get_millis());
	while (1) {

		// Copy current height from ADC
        current_alt =  (-1 * adc_get_percent()+55)*4;
        current_yaw = quad_get_degrees();

		ButtonStatus flight_mode = button_status(button_flight_mode);

		switch ( program_state ) {
			case CALIBRATION: {
				// Record minimum height
				// Ramp PWM until altitude starts moving, record alt-hold value
				// Find maximum height
				// Rotate using yaw and find reference point
				// Ramp to ground

				pwm_duty_cycle_set(&yaw_output, 2 + 10 * ((float) timer_get_lap()/1000));

				if (2 + 10 * ((float) timer_get_lap()/1000) > 30) {
					DEBUG("Calibration Completed in %d ms. Landed", timer_get_lap());
					pwm_set_state(yaw_output, false);
					calibrated = true;
					program_state = LANDED;
				}

			} break;
			case LANDED: {
				// If button_flight_mode has turned on,
				// Set to Flying mode.
				// Otherwise wait
				if (flight_mode == RELEASE_EVENT) {
					// Flight Mode Switch has been set to "flight"
					if (calibrated) {
						DEBUG("Flying");
						program_state = FLYING;
					} else {
						DEBUG("Beginning calibration...");
						pwm_duty_cycle_set(&yaw_output, 2);
						pwm_set_state(yaw_output, true);
						timer_set_lap();
						program_state = CALIBRATION;
					}
				}
			} break;
			case FLYING: {

				// If button_flight_mode has turned off, switch to landing mode
				if (flight_mode == PRESS_EVENT) {
					// Flight Mode switch has been set to "land"
					DEBUG("Landing...");
					program_state = LANDING;
					break;
				}
				// Read alt and yaw controls to update controller targets
				if (button_status(button_alt_up) == PRESS_EVENT) {
		                DEBUG("Altitude up button pressed");
					  //  pid_target_set(&alt_output, pid_alt.target + ALT_INCREMENT);
				} else if (button_status(button_alt_down) == PRESS_EVENT) {
		                DEBUG("Altitude down button pressed");
					   // pid_target_set(&pid_alt, pid_alt.target - ALT_DECREMENT);
				}
				if (button_status(button_yaw_left) == PRESS_EVENT) {
		                DEBUG("Yaw left button pressed");
					    //pid_target_set(&pid_yaw, pid_yaw.target + YAW_INCREMENT);
				} else if (button_status(button_yaw_right) == PRESS_EVENT) {
		                DEBUG("Yaw right button pressed");
					    //pid_target_set(&pid_yaw, pid_yaw.target - YAW_DECREMENT);
				}

			} break;
			case LANDING: {
				// Ignore inputs from user
				// Set target yaw to 0,
				// Give alt target a nice time-dependant smooth landing
				// When altitude reaches 0, switch off motors, and switch to LANDED mode
				// If flying switch turned back on, set targets to current height/yaw, return to FLYING mode
				if (flight_mode == RELEASE_EVENT) {

					// TODO: Set alt and yaw targets to current values (pause movement)
					DEBUG("Landing abort. Flying");
					program_state = FLYING;
					break;
				}
				if (current_alt < 10) {
					DEBUG("Landed");
					program_state = LANDED;
					break;
				}
				// Update ramp target
				// Update PWM from PID controllers

			} break;
		}

        if (current_alt != previous_alt) {
        	  if (timer_get_millis() % 100 == 0) {
				// UART print round, format data for the humans
				display_update_alt(current_alt);
				previous_alt = current_alt;
        	  }
        }

        if (timer_get_millis() % 1000 == 0) {
        	// UART print round, format data for the humans

        }
	}
}

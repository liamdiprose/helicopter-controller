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

// Define possible states of program overall FSM
typedef enum {
	CALIBRATION, LANDED, FLYING, LANDING
} ProgramState;

enum calibration_states {
    	CALIBRATE_HEIGHT, CALIBRATE_YAW
};

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

// Reset Button
#define BUTTON_RESET_PERIPH SYSCTL_PERIPH_GPIOA
#define BUTTON_RESET_BASE GPIO_PORTA_BASE
#define BUTTON_RESET_PIN GPIO_PIN_6

// Flight Mode Switch
#define SWITCH_FMODE_PERIPH SYSCTL_PERIPH_GPIOA
#define SWITCH_FMODE_BASE GPIO_PORTA_BASE
#define SWITCH_FMODE_PIN GPIO_PIN_7

// Yaw reference "button"
#define BUTTON_YAW_REF_PERIPH SYSCTL_PERIPH_GPIOC
#define BUTTON_YAW_REF_BASE GPIO_PORTC_BASE
#define BUTTON_YAW_REF_PIN GPIO_PIN_4

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

#define YAW_STEP 15
#define ALT_STEP 10

// CSV Status Options
#define CSV_STATUS_PRINTING 1
#define STATUS_CSV_HEADER "Target Yaw, Actual Yaw, Target Altitude, Actual Altitude, Tail Rotor Duty Cycle, Main Rotor Duty Cycle, Operating Mode"
#define STATUS_CSV_FORMAT "%d, %d, %d, %d, %d, %d, %s\r\n"
#if CSV_STATUS_PRINTING
#define CSV_STATUS(...) UARTprintf(STATUS_CSV_FORMAT, ##__VA_ARGS__)
#else
#define CSV_STATUS(...) do {} while (0)
#endif




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
	PIDConfig pid_alt = pid_init(1, 0, 0.1);
    PIDConfig pid_yaw = pid_init(1, 0, 0.1);
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
    ButtonStatus flight_mode;

    // Track if program has calibrated yet
    bool calibrated = false;
    enum calibration_states calibration_state;

    uint32_t altitude_low = 55;

	// Declear variables for controllers
    bool controllers_enabled = false;
	uint32_t current_alt;
	uint32_t current_yaw;
	uint32_t target_alt = 0;
	uint32_t target_yaw = 0;
	uint32_t alt_error;
	uint32_t yaw_error;
	uint32_t alt_dc;
	uint32_t yaw_dc;

	float time_delta = 0.001;

	// Save variables to detect changes inbetween loops
    uint32_t previous_alt = 1;


    SysTickIntRegister(all_the_routines);
	SysTickEnable();
	SysTickIntEnable();

	uint32_t last_loop = timer_get_millis();

    DEBUG("Startup completed in %d ms", timer_get_millis());
	while (1) {
		// Copy current height from ADC
		if (calibrated) {
			current_alt =  (-1 * adc_get_percent()+altitude_low)*4;
		} else {
			current_alt = adc_get_percent();
		}
        current_yaw = quad_get_degrees();

		flight_mode = button_status(button_flight_mode);

		switch ( program_state ) {
			case CALIBRATION: {

				switch ( calibration_state ) {
				case CALIBRATE_HEIGHT: {
						altitude_low = current_alt;  // TODO: Samples
						DEBUG("Calibrating height, lowest height is %d%%", altitude_low);
						program_state = LANDED;
						calibrated = true;
					}
				}

				pwm_duty_cycle_set(&yaw_output, 2 + 10 * ((float) timer_get_lap()/1000));

				if (2 + 10 * ((float) timer_get_lap()/1000) > 30) {
					DEBUG("Calibration Completed in %d ms. Landed", timer_get_lap());
					pwm_set_state(alt_output, false);
					pwm_set_state(yaw_output, false);
					calibrated = true;
					program_state = LANDED;
				}
			} break;
			case LANDED: {
				if (flight_mode == RELEASE_EVENT) {
					// Begin flight


					if (calibrated) {
						DEBUG("Flying");
						pwm_duty_cycle_set(&alt_output, 2);
						pwm_duty_cycle_set(&yaw_output, 2);
						pwm_set_state(yaw_output, true);
						controllers_enabled = true;
						program_state = FLYING;
					} else {
						DEBUG("Beginning calibration...");
						timer_set_lap();
						calibration_state = CALIBRATE_HEIGHT;
						controllers_enabled = true;
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
					    if (target_alt < 100) {
					    	target_alt += ALT_STEP;
					    }
				}
				if (button_status(button_alt_down) == PRESS_EVENT) {
		                DEBUG("Altitude down button pressed");
		                if (target_alt > 0) {
		                	target_alt -= ALT_STEP;
		                }
				}
				if (button_status(button_yaw_left) == PRESS_EVENT) {
		                DEBUG("Yaw left button pressed");
		                if (target_alt < 360) {
		                	target_yaw += YAW_STEP;
		                }
				}
				if (button_status(button_yaw_right) == PRESS_EVENT) {
		                DEBUG("Yaw right button pressed");
		                if (target_yaw > 0) {
		                	target_yaw -= YAW_STEP;
		                }
				}

			} break;
			case LANDING: {
				if (flight_mode == RELEASE_EVENT) {
					DEBUG("Landing abort. Flying");

					// Seize Movement
					target_alt = current_alt;
					target_yaw = current_yaw;

					program_state = FLYING;
					break;
				}
				if (current_alt < altitude_low + 10) {
					DEBUG("Landed");
					pwm_set_state(alt_output, false);
					pwm_set_state(yaw_output, false);
					controllers_enabled = false;
					program_state = LANDED;
				}
			} break;
		}

		if (controllers_enabled) {
			// Plug controllers into rotors
			time_delta = (float) timer_get_lap() / 1000;

			alt_error = target_alt - current_alt;
			yaw_error = target_yaw - current_yaw; // TODO: Go other way

			alt_dc = pid_update(&pid_alt, alt_error, time_delta);
			yaw_dc = pid_update(&pid_yaw, yaw_error, time_delta);

			pwm_duty_cycle_set(&alt_output, alt_dc);
			pwm_duty_cycle_set(&yaw_output, yaw_dc);

			timer_set_lap();
		}

        if (current_alt != previous_alt) {
        	  if (timer_get_millis() % 100 == 0) {
				// UART print round, format data for the humans
				display_update_alt(current_alt);
				previous_alt = current_alt;
        	  }
        }

        if (timer_get_millis() % 250 == 0) {
        	// Target Yaw, Actual Yaw, Target Altitude, Actual Altitude, Tail Rotor Duty Cycle, Main Rotor Duty Cycle, Operating Mode

        //	CSV_STATUS(target_yaw, current_yaw, target_alt, current_alt,  yaw_output.duty_cycle,
		//			alt_output.duty_cycle, program_state);

        	alt_dc = 100 * (float) alt_output.duty_cycle / alt_output.period;
        	yaw_dc = 100 * (float) yaw_output.duty_cycle / yaw_output.period;


        	UARTprintf("T:%dY:%d[%d]A:%d[%d]T:%dM:%dTD:%d\n", timer_get_millis(), current_yaw, target_yaw, current_alt, target_alt, yaw_dc, alt_dc, 0.001*1000);

        }

	}
}

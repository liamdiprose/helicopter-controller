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

#include "config.h"

// Define possible states of program overall FSM
typedef enum {
	CALIBRATION, LANDED, FLYING, LANDING
} ProgramState;

enum calibration_state_e {
    	CALIBRATE_HEIGHT, CALIBRATE_YAW
};


#define ALTITUDE_HOLD 0
#define YAW_HOLD 0

#define YAW_STEP 15
#define ALT_STEP 10

#define CALIBRATION_DEG_PER_SEC 10

#define TIMEPERIOD_CONTROLLER 0
#define TIMEPERIOD_CALIBRATION 1

void clock_init(void) {
	SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}

void all_the_routines(void) {
	 button_check_routine();
	 adc_update_routine();
}

// Run all modules init functions
void main_setup(void) {
	clock_init();


	SysTickPeriodSet(SysCtlClockGet()/100);

	// Start timer
	timer_init();

	// Initiase UART
	uart_init();
	DEBUG("UART Initialised");
	// Start ADC module to read altitude
	adc_init();
	DEBUG("ADC Initialised");

	// Start quadrature encoder to read yaw
	quad_init();
	DEBUG("Quadrature Encoder Initialised");

	// Start display to display TODO information
	display_init();
	DEBUG("Display Initialised");

	SysTickIntRegister(all_the_routines);
	SysTickEnable();
	SysTickIntEnable();
}


int main(void) {
	main_setup();

	//////////////////////////////////////////////////////////////////////////////////////
	// Initialise variables needed for main program loop
	//////////////////////////////////////////////////////////////////////////////////////

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
	Button* button_reset = button_init ( BUTTON_RESET_PERIPH, BUTTON_RESET_BASE,
					BUTTON_RESET_PIN, PULL_DOWN);  // TODO: Pull up?
	Button* button_yaw_reference = button_init(BUTTON_YAW_REF_PERIPH, BUTTON_YAW_REF_BASE,
					BUTTON_YAW_REF_PIN, PULL_DOWN);


    // Create PID controllers
	PIDConfig pid_alt = pid_init(0, 0.01, 0);
    PIDConfig pid_yaw = pid_init(0.08, 0.01, 0);//  0.0002, 0.00000005);

    // Create PWM outputs to control rotors
	PWMOut alt_output = pwm_init(PWM_ALT_PWM_PERIPH, PWM_ALT_PWM_BASE, PWM_ALT_GEN, PWM_ALT_OUT, PWM_ALT_OUTBIT);
    pwm_init_gpio(&alt_output, PWM_ALT_GPIO_PERIPH, PWM_ALT_GPIO_BASE, PWM_ALT_GPIO_PIN, PWM_ALT_GPIO_PIN_CONF);

	PWMOut yaw_output = pwm_init(PWM_YAW_PWM_PERIPH, PWM_YAW_PWM_BASE, PWM_YAW_GEN, PWM_YAW_OUT, PWM_YAW_OUTBIT);
    pwm_init_gpio(&yaw_output, PWM_YAW_GPIO_PERIPH, PWM_YAW_GPIO_BASE, PWM_YAW_GPIO_PIN, PWM_YAW_GPIO_PIN_CONF);
    DEBUG("PWM outputs Initialised");

    // Track state of program FSM
    ProgramState program_state = LANDED;

    // Hold the status of the flight mode switch
    ButtonStatus flight_mode;

    // Track if program has calibrated yet
    enum calibration_state_e calibration_state;
    bool calibrated = false;


    uint32_t altitude_low = 55;

	// Declear variables for controllers
    bool controllers_enabled = false;
	int32_t current_alt = 0;
	int32_t current_yaw = 0;
	uint32_t target_alt = 0;
	int32_t target_yaw = 0;
	int32_t alt_error;
	int32_t yaw_error;
	float alt_dc;
	float yaw_dc;
	float old_alt_dc = 0;
	float old_yaw_dc = 0;

	float time_delta;

	// Count loops to schedule UART and Display
	uint32_t loop_count = 0;


    DEBUG("Startup completed in %d ms", timer_get_micros()/1000);
	while (1) {
		// Copy current height from ADC
		if (calibrated) {
			current_alt =  (-1 * adc_get_percent()+altitude_low)*4;
		} else {
			current_alt = adc_get_percent();
		}

        current_yaw = quad_get_degrees();

		flight_mode = button_status(button_flight_mode);

//		if (button_status(button_reset) == PRESS_EVENT) {
//				DEBUG("Reset button pressed");
//				SysCtlReset();
//		}

		switch ( program_state ) {
			case CALIBRATION: {

				switch ( calibration_state ) {
				case CALIBRATE_HEIGHT: {
						altitude_low = adc_get_percent();  // TODO: Samples
						pid_clear_errors(&pid_alt);
						pid_clear_errors(&pid_yaw);
						controllers_enabled = false;
						DEBUG("Calibrating height, lowest height is %d%%", altitude_low);
						calibration_state = CALIBRATE_YAW;
						calibrated = true;
						program_state = LANDED;
						timer_record(TIMEPERIOD_CALIBRATION);
					}; break;
					case CALIBRATE_YAW: {
						controllers_enabled = true;
						target_yaw = (int32_t) CALIBRATION_DEG_PER_SEC * timer_seconds_since(TIMEPERIOD_CALIBRATION);
						if (button_status(button_yaw_reference) == PRESS_EVENT) {
							// Yaw reference found
							DEBUG("Found Yaw reference. Reset current yaw");
							current_yaw = 0;
							program_state = LANDED;
							pwm_set_state(alt_output, false);
							pwm_set_state(yaw_output, false);
							calibrated = true;
						}
					}
				}
			} break;
			case LANDED: {
				if (flight_mode == RELEASE_EVENT) {
					// Begin flight

					if (calibrated) {
						DEBUG("Flying");
						pwm_duty_cycle_set(&alt_output, 0);
						pwm_duty_cycle_set(&yaw_output, 0);
						pwm_set_state(alt_output, true);
						pwm_set_state(yaw_output, true);
						controllers_enabled = true;
						program_state = FLYING;
					} else {
						DEBUG("Beginning calibration...");
						timer_record(TIMEPERIOD_CALIBRATION);
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
		                target_yaw += YAW_STEP;
				}
				if (button_status(button_yaw_right) == PRESS_EVENT) {
		                DEBUG("Yaw right button pressed");
		                target_yaw -= YAW_STEP;
				}

			} break;
			case LANDING: {
				if (current_alt < altitude_low + 10) {
					DEBUG("Landed");
					target_alt = 0;
					target_yaw = current_yaw;

					pwm_set_state(alt_output, false);
					pwm_set_state(yaw_output, false);
					pid_clear_errors(&pid_alt);
					pid_clear_errors(&pid_yaw);
					controllers_enabled = false;
					program_state = LANDED;
				} else {
					// TODO: Soft landing at yaw=0
					target_yaw = 0;

					// Save time variable for time_delta.
					// Compute height for time
				}
			} break;
		}

		if (controllers_enabled) {
			// Plug controllers into rotors

				time_delta = timer_seconds_since(TIMEPERIOD_CONTROLLER);

				alt_error = target_alt - current_alt;
				yaw_error = target_yaw - current_yaw;

				alt_dc = pid_update(&pid_alt, alt_error, time_delta);

				if (alt_dc != old_alt_dc) {
					alt_dc += ALTITUDE_HOLD;
					pwm_duty_cycle_set(&alt_output, alt_dc);
					old_alt_dc = alt_dc;
				}
				yaw_dc = pid_update(&pid_yaw, yaw_error, time_delta);
				if (yaw_dc != old_yaw_dc) {
					yaw_dc += YAW_HOLD;

					pwm_duty_cycle_set(&yaw_output, yaw_dc);
					old_yaw_dc = yaw_dc;
				}

				timer_record(TIMEPERIOD_CONTROLLER);
		}

		  if (timer_get_micros() / 1000 % 50 == 0) {
			display_update(current_alt, target_alt, current_yaw, target_yaw);
		  }

        if (timer_get_micros() / 10000 % 50 == 0) {

        	int32_t p_alt_dc = (uint32_t) (alt_dc * 100);
        	//int32_t p_yaw_dc = (uint32_t) (yaw_output.duty_cycle * 100);
        	int32_t p_yaw_dc = (uint32_t) (yaw_dc * 100);
        	int32_t p_perr = pid_yaw.KP * yaw_error * time_delta * 100;
        	int32_t p_ierr = pid_yaw.KI * ((float) pid_yaw.I_error * time_delta) * 100;
        	int32_t p_derr = pid_yaw.KD * ((float) pid_yaw.D_error / time_delta) * 100;
        	int32_t p_cyaw = current_yaw;

        //	uint32_t milliseconds = (int32_t) (time_delta * 1000000);

        	UARTprintf("[A c:%d|t:%d|e:%d|dc:%d]\r\n[Y c:%d|t:%d|e:%d|dc:%d=(P:%d|I:%d|D:%d)]\n", current_alt, target_alt, alt_error, p_alt_dc, p_cyaw, target_yaw, yaw_error, p_yaw_dc, p_perr, p_ierr, p_derr);

        }
        if (loop_count == 0x7FFFFFFF) {
        	DEBUG("Reseting loop count");
        	loop_count ++;
        } else {
        	loop_count = 0;
        }

       }
}


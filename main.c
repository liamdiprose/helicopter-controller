#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"

#include "button.h"
#include "pwm.h"
#include "pid.h"
#include "quad_encoder.h"
#include "adc.h"
#include "uart.h"
#include "display.h"
#include "timer.h"
#include "debug.h"

#define BUTTON_ALT_UP_PERIPH SYSCTL_PERIPH_GPIOE
#define BUTTON_ALT_UP_BASE GPIO_PORTE_BASE
#define BUTTON_ALT_UP_PIN GPIO_PIN_0

#define BUTTON_ALT_DOWN_PERIPH SYSCTL_PERIPH_GPIOD
#define BUTTON_ALT_DOWN_BASE GPIO_PORTD_BASE
#define BUTTON_ALT_DOWN_PIN GPIO_PIN_2

#define BUTTON_YAW_LEFT_PERIPH SYSCTL_PERIPH_GPIOF
#define BUTTON_YAW_LEFT_BASE GPIO_PORTF_BASE
#define BUTTON_YAW_LEFT_PIN GPIO_PIN_0

#define BUTTON_YAW_RIGHT_PERIPH SYSCTL_PERIPH_GPIOF
#define BUTTON_YAW_RIGHT_BASE GPIO_PORTF_BASE
#define BUTTON_YAW_RIGHT_PIN GPIO_PIN_4

#define SWITCH_FMODE_PERIPH SYSCTL_PERIPH_GPIOA
#define SWITCH_FMODE_BASE GPIO_PORTA_BASE
#define SWITCH_FMODE_PIN GPIO_PIN_7

#define PWM_ALT_GEN PWM_GEN_3
#define PWM_ALT_OUT PWM_OUT_6
#define PWM_ALT_PWM_PERIPH SYSCTL_PERIPH_PWM0
#define PWM_ALT_PWM_BASE PWM0_BASE
#define PWM_ALT_OUTBIT PWM_OUT_0_BIT
#define PWM_ALT_GPIO_PERIPH SYSCTL_PERIPH_GPIOC
#define PWM_ALT_GPIO_BASE GPIO_PORTC_BASE
#define PWM_ALT_GPIO_PIN GPIO_PIN_5


void clock_init(void) {
	SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}


void gpiof_unlock() {

}

int main(void) {
	
    clock_init();

	SysTickPeriodSet(SysCtlClockGet()/100);

    // Initiase UART
	uart_init();
    DEBUG("UART Initialised");
    timer_init();


	// Initialise buttons
    button_init();

	Button* button_alt_up = button_create(BUTTON_ALT_UP_PERIPH, BUTTON_ALT_UP_BASE,
                    BUTTON_ALT_UP_PIN, PULL_UP);
	Button* button_alt_down = button_create(BUTTON_ALT_DOWN_PERIPH, BUTTON_ALT_DOWN_BASE,
                    BUTTON_ALT_DOWN_PIN, PULL_UP);
	Button* button_yaw_left = button_create(BUTTON_YAW_LEFT_PERIPH, BUTTON_YAW_LEFT_BASE,
                    BUTTON_YAW_LEFT_PIN, PULL_DOWN);
	Button* button_yaw_right = button_create(BUTTON_YAW_RIGHT_PERIPH, BUTTON_YAW_RIGHT_BASE,
                    BUTTON_YAW_RIGHT_PIN, PULL_DOWN);
	Button* button_flight_mode = button_create(SWITCH_FMODE_PERIPH, SWITCH_FMODE_BASE,
	    				SWITCH_FMODE_PIN, NONE);
	DEBUG("Buttons Initialised");


	adc_init();
    DEBUG("ADC Initialised");

	quad_init();
    DEBUG("Quadrature Encoder Initialised");

	PIDConfig pid_alt = pid_init(1, 0, 0.01);
    PIDConfig pid_yaw = pid_init(1, 0, 0.01);
    DEBUG("PID Controllers Initialised");


	PWMOut alt_output = pwm_init(PWM_ALT_PWM_PERIPH, PWM_ALT_PWM_BASE, PWM_ALT_GEN, PWM_ALT_OUT, PWM_ALT_OUTBIT);
    pwm_init_gpio(&alt_output, PWM_ALT_GPIO_PERIPH, PWM_ALT_GPIO_BASE, PWM_ALT_GPIO_PIN);
    pwm_set_state(alt_output, PWM_STATE_ON);


	//PWMOut yaw_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, PWM_DEFAULT_FREQUENCY, 0);
    DEBUG("PWM outputs Initialised");

    display_init();
    DEBUG("Display Initialised");

	// Declear variables that hold sensor input
	uint32_t current_alt;
	uint32_t current_yaw;

    uint32_t previous_alt = 1;



    SysTickIntRegister(button_check_routine);
	SysTickEnable();
	SysTickIntEnable();


    DEBUG("Timer started %d units ago", timer_get_millis());
	while (1) {


		// Copy current height from ADC
        current_alt = 4; //adc_get_percent();
        current_yaw = quad_get_degrees();

        // Ask each button if they have been pressed, and if so,
        // perform specific action assigned to them.
		if (button_status(button_alt_up) == PRESS_EVENT) {
                // Altitude Up button Process
                DEBUG("Altitude up button pressed. New Altitude: %d", pid_alt.target);
			  //  pid_target_set(&alt_output, pid_alt.target + ALT_INCREMENT);

		} else if (button_status(button_alt_down) == PRESS_EVENT) {
                DEBUG("Altitude down button pressed. New Altitude: %d", pid_alt.target);
			   // pid_target_set(&pid_alt, pid_alt.target - ALT_DECREMENT);
		}
		if (button_status(button_yaw_left) == PRESS_EVENT) {
                DEBUG("Yaw left button pressed. New Yaw: %d", pid_yaw.target);
			    //pid_target_set(&pid_yaw, pid_yaw.target + YAW_INCREMENT);
		} else if (button_status(button_yaw_right) == PRESS_EVENT) {
                DEBUG("Yaw right button pressed. New Yaw: %d", pid_yaw.target);
			    //pid_target_set(&pid_yaw, pid_yaw.target - YAW_DECREMENT);
		}

		ButtonStatus flight_mode = button_status(button_flight_mode);
		if (flight_mode == PRESS_EVENT || flight_mode == RELEASE_EVENT) {
			char* fmode = flight_mode == PRESS_EVENT ? "Fly" : "Land";

			DEBUG("Flight Mode toggled to: %s", fmode);
		}

        //pwm_duty_cycle_set(&alt_output, pid_update(&pid_alt, 0, current_alt));
        //pwm_duty_cycle_set(&yaw_output, pid_update(&yaw_alt, current_yaw));

        if (current_alt != previous_alt) {
		    display_update_alt(current_alt);
		    previous_alt = current_alt;
        }
	}
}

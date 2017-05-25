#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"

#include "button.h"
#include "pwm.h"
#include "pid.h"
#include "quad_encoder.h"
#include "adc.h"
#include "uart.h"
#include "display.h"

#include "debug.h"

void clock_init(void) {
	SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}


int main(void) {
	
    clock_init();

    // Initiase UART
	uart_init();
    DEBUG("UART Initialised");

	// Initialise buttons
	Button* button_alt_up = button_init(BUTTON_ALT_UP_PERIPH, BUTTON_ALT_UP_BASE,
                    BUTTON_ALT_DOWN_PIN);
	Button* button_alt_down = button_init(BUTTON_ALT_DOWN_PERIPH, BUTTON_ALT_DOWN_BASE,
                    BUTTON_ALT_DOWN_PIN);
	Button* button_yaw_left = button_init(BUTTON_YAW_LEFT_PERIPH, BUTTON_YAW_LEFT_BASE,
                    BUTTON_YAW_RIGHT_PIN);
	Button* button_yaw_right = button_init(BUTTON_YAW_RIGHT_PERIPH, BUTTON_YAW_RIGHT_BASE,
                    BUTTON_YAW_RIGHT_PIN);
    DEBUG("Buttons Initialised");


	adc_init();
    DEBUG("ADC Initialised");

	quad_init();
    DEBUG("Quadrature Encoder Initialised");

	PIDConfig pid_alt = pid_init(1, 0, 0.01);
    PIDConfig pid_yaw = pid_init(1, 0, 0.01);
    DEBUG("PID Controllers Initialised");

    //PWMOut pwm_init(uint32_t addr_base, uint32_t clk_gen, uint32_t out, uint32_t outbit) {

	PWMOut alt_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, PWM_DEFAULT_FREQUENCY);
    pwm_init_gpio(&alt_output, PWM_ALT_GPIO_PERIPH, PWM_ALT_GPIO_BASE, PWM_ALT_GPIO_PIN);

	//PWMOut yaw_output = pwm_init(PWM0_BASE, PWM_GEN_3, PWM_OUT_6, PWM_DEFAULT_FREQUENCY, 0);
    DEBUG("PWM outputs Initialised");

	// Declear variables that hold sensor input
	uint32_t current_alt;
	uint32_t current_yaw;

    uint32_t previous_alt = 1;

    DEBUG("Starting main loop");
	while (1) {
		// Copy current height from ADC
        current_alt = alt_get_percentage();
        current_yaw = quad_get_degrees();

        // Ask each button if they have been pressed, and if so,
        // perform specific action assigned to them.
		if (button_read(button_alt_up)) {
                // Altitude Up button Process
                DEBUG("Altitude up button pressed. New Altitude: %d"/*, pid_alt->target*/);
			  //  pid_target_set(&alt_output, pid_alt.target + ALT_INCREMENT);

		} else if (button_alt_down) {
                DEBUG("Altitude down button pressed. New Altitude: %d"/*, pid_alt->target*/);
			   // pid_target_set(&pid_alt, pid_alt.target - ALT_DECREMENT);
		}

		if (button_yaw_left) {
                DEBUG("Yaw left button pressed. New Yaw: %d"/*, pid_yaw->target*/);
			    //pid_target_set(&pid_yaw, pid_yaw.target + YAW_INCREMENT);
		} else if (button_yaw_right) {
                DEBUG("Yaw right button pressed. New Yaw: %d"/*, pid_yaw->target*/);
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

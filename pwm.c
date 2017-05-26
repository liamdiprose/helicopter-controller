
#include "pwm.h"


// Create a PWM output 
PWMOut pwm_init(uint32_t pwm_periph, uint32_t addr_base, uint32_t clk_gen, uint32_t out, uint32_t outbit) {

        SysCtlPeripheralEnable(pwm_periph);
        while(!SysCtlPeripheralReady(pwm_periph));

        // Prescale the PWM Clock by 8
	    SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

		PWMGenConfigure(addr_base, clk_gen, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
		PWMGenEnable(addr_base, clk_gen);

		PWMOut new_pwm_out = {
				.base = addr_base,
				.gen = clk_gen,
				.out = out,
				.outbit = outbit
		};

		pwm_frequency_set(&new_pwm_out, 150);
		pwm_duty_cycle_set(&new_pwm_out, 50);

		PWMOutputState(addr_base, outbit, false);

		// Return struct for handling this PWM output
		return new_pwm_out;
}

// Initialise GPIO output for PWM generator
void pwm_init_gpio(PWMOut* pwm_out, uint32_t gpio_periph, uint32_t gpio_port, uint32_t gpio_pin_conf) {
        SysCtlPeripheralEnable(gpio_periph);
        while(!SysCtlPeripheralReady(gpio_periph));
        GPIOPinConfigure(gpio_pin_conf);
}

// Turn the PWM on or off
void pwm_set_state(PWMOut pin, bool new_state) {
	PWMOutputState(pin.base, pin.outbit, new_state);
}


// Set the duty cycle of the pwm output
void pwm_duty_cycle_set(PWMOut* pin, uint8_t duty_cycle) {
		uint32_t new_pulse_width = pin->period * ( (float) duty_cycle / 100 );

		PWMPulseWidthSet(pin->base, pin->out, new_pulse_width);
        pin->duty_cycle = new_pulse_width;
}

// Set the frequncy of the pwm output
void pwm_frequency_set(PWMOut* pin, uint32_t frequency) {
		uint32_t period =  SysCtlClockGet() / 8 / frequency;

		PWMGenPeriodSet(pin->base, pin->gen, period);
        // Update the PWM
        pwm_duty_cycle_set(pin, pin->duty_cycle);

		pin->period = period;
}


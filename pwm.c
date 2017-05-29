
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

		pwm_frequency_set(&new_pwm_out, PWM_DEFAULT_FREQUENCY);

		PWMOutputState(addr_base, outbit, false);

		// Return struct for handling this PWM output
		return new_pwm_out;
}

// Initialise GPIO output for PWM generator
void pwm_init_gpio(PWMOut* pwm_out, uint32_t periph, uint32_t port, uint32_t pin, uint32_t pin_conf) {
        SysCtlPeripheralEnable(periph);
        while(!SysCtlPeripheralReady(periph));
        GPIOPinTypePWM(port, pin);
        GPIOPinConfigure(pin_conf);
}

// Turn the PWM on or off
void pwm_set_state(PWMOut pin, bool new_state) {
	PWMOutputState(pin.base, pin.outbit, new_state);
}


// Set the duty cycle of the pwm output
void pwm_duty_cycle_set(PWMOut* pin, float duty_cycle) {
		// Cap output to protect rotor motors
		if (duty_cycle > PWM_DUTY_CYCLE_MAX) {
			duty_cycle = PWM_DUTY_CYCLE_MAX;
		} else if (duty_cycle < PWM_DUTY_CYCLE_MIN) {
			duty_cycle = PWM_DUTY_CYCLE_MIN;
		}

		uint32_t new_pulse_width = pin->period * duty_cycle;

		PWMPulseWidthSet(pin->base, pin->out, new_pulse_width);
        pin->duty_cycle = duty_cycle;
}

// Set the frequncy of the pwm output
void pwm_frequency_set(PWMOut* pin, uint32_t frequency) {
		uint32_t period =  SysCtlClockGet() / 8 / frequency;

		PWMGenPeriodSet(pin->base, pin->gen, period);
        // Update the PWM
        pwm_duty_cycle_set(pin, period * pin->duty_cycle);

		pin->period = period;
}


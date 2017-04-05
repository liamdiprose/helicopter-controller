
#include "pwm.h"


// Create a PWM output handler
PWMOut pwm_init(uint32_t addr_base, uint32_t clk_gen, uint32_t out, uint32_t outbit){

		PWMGenConfigure(addr_base, clk_gen, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

		PWMGenEnable(addr_base, clk_gen);

		PWMOut new_pwm_out = {
				.base = addr_base,
				.gen = clk_gen,
				.out = out,
				.outbit = outbit,
		};

		pwm_frequency_set(&new_pwm_out, 150);
		pwm_duty_cycle_set(&new_pwm_out, 50);

		PWMOutputState(addr_base, outbit, true);

		// Return struct for handling this PWM output
		return new_pwm_out;
}

// Set the duty cycle of the pwm output
int pwm_duty_cycle_set(PWMOut* pin, uint8_t duty_cycle) {
		//uint32_t pulse_width = SysCtlClockGet() / 4 / pin->frequency;

		PWMPulseWidthSet(pin->base, pin->out, pin->period * ((float)duty_cycle/100.0));

		pin->duty_cycle = duty_cycle;
		return 0;
}

// Set the frequncy of the pwm output
int pwm_frequency_set(PWMOut* pin, uint32_t frequency) {
		uint32_t period =  SysCtlClockGet() / 8 / frequency;

		PWMGenPeriodSet(pin->base, pin->gen, period);
		PWMPulseWidthSet(pin->base, pin->out, period * ((float)pin->duty_cycle/100.0));

		pin->period = period;
		return 0;
}


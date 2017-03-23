
// Create a PWM output handler
PWMOut pwm_init(uint32_t addr_base, uint32_t clk_gen, uint32_t out, uint32_t outbit);

		PWMGenConfigure(addr_base, clk_gen, 
						PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

		PWMGenEnable(addr_base, clk_gen);
		PWMOutputState(addr_base, outbit, true);

		// Return struct for handling this PWM output 
		return {
				.base = addr_base,
				.gen = clk_gen,
				.out = out,
				.outbit = outbit,
				.frequency = 150,
				.duty_cycle = 0.5
		};
}

// Set the duty cycle of the pwm output
int pwm_duty_cycle_set(PWMOut* pin, float duty_cycle) {
		uint32_t pulse_width = SysCtlClockGet() / PWM_DIVIDER / pwm->frequency;

		PWMPulseWidthSet(pwm->base, pwm->out, pulse_width);

		pin->duty_cycle = duty_cycle;
		return 0;
}

// Set the frequncy of the pwm output
int pwm_frequency_set(PWMOut* pin, int frequency) {
		uint32_t period = SysCtlClockGet() / PWM_DIVIDER / frequency;

		PWMGenPeriodSet(pin->base, pwm->gen, period);

		pin->frequency = frequency;
		return 0;
}


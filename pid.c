#include <stdint.h>

#include "pid.h"

// Create a new PID configuration
PIDConfig pid_init(float KP, float KI, float KD) {
	PIDConfig config = {KP, KI, KD};
	return config;
}

// Calculate new motor throttle percentage for PID config
uint32_t pid_update(PIDConfig* config, uint32_t error, float dt) {
	// Calculate derivative and intergral errors
	config->I_error = config->I_error + error;
	config->D_error = error - config->D_error;

	return config->KP * error
			+ config->KI * (config->I_error * dt)
			+ config->KD * (config->D_error / dt);
}

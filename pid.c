#include <stdint.h>

#include "pid.h"
#include "debug.h"

// Create a new PID configuration
PIDConfig pid_init(float KP, float KI, float KD) {
	PIDConfig config = {KP, KI, KD, 0, 0};
	return config;
}

void pid_clear_errors(PIDConfig* config) {
	config->D_error = 0;
	config->I_error = 0;

}

// Calculate new motor throttle percentage for PID config
float pid_update(PIDConfig* config, int32_t error, float dt) {
	// Calculate derivative and intergral errors

	config->I_error += error;

	config->D_error = error - config->D_error;

	return config->KP * error
			+ config->KI * ((float) config->I_error * dt)
			+ config->KD * ((float) config->D_error / (0.01 + dt));
}

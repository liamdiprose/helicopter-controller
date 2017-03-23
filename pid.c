/*
 * pid.c
 *
 *  Created on: Mar 23, 2017
 *      Author: ldi30
 */
#include <stdint.h>

#include "pid.h"

// Create a new PID configuration
PIDConfig pid_init(float KP, float KI, float KD) {
	PIDConfig config = {KP, KI, KD, 0};
	return config;
}

// Set Proportional Gain (KP) of PID configuration
int pid_proportional_set(PIDConfig* config, float KP) {
	config->P = KP;
	return 0;
}

// Set Intergral Gain (KI) of PID configuration
int pid_intergral_set(PIDConfig* config, float KI) {
	config->I = KI;
	return 0;
}

// Set Derivative Gain (KD) of PID configuration
int pid_derivative_set(PIDConfig* config, float KD) {
	config->D = KD;
	return 0;
}

int pid_target_set(PIDConfig* config, float target) {
	config->target = target;
	return 0;
}

// Calculate new value from PID controller
float pid_update(PIDConfig config, uint32_t current) {
	// TODO: Just return the target value for now...
	return config.target;
}

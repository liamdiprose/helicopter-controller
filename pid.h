/*
 * pid.h
 *
 *  Created on: Mar 22, 2017
 *      Author: Aden Durney
 *      Author: Liam Diprose
 */

#ifndef PID_H_
#define PID_H_


typedef struct PID_config_s {
	float KP;
	float KI;
	float KD;
	float I_error;
	float D_error;
} PIDConfig;

// Create a new PID configuration
PIDConfig pid_init(float KP, float KI, float KD);

void pid_clear_errors(PIDConfig* config);

// Calculate new duty cycle output from PID controller, from target and
// current reading.
float pid_update(PIDConfig* config, int32_t error, float dt);

#endif /* PID_H_ */

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
	float P;
	float I;
	float D;
	float target;
} PIDConfig;

// Create a new PID configuration
PIDConfig pid_init(float KP, float KI, float KD);

// Set Proportional Gain (KP) of PID configuration
int pid_proportional_set(PIDConfig config, float KP);

// Set Intergral Gain (KI) of PID configuration
int pid_intergral_set(PIDConfig config, float KI);

// Set Derivative Gain (KD) of PID configuration
int pid_derivative_set(PIDConfig config, float KD);

// Calculate new value from PID controller
float pid_update(PIDConfig config, float current, float target);

#endif /* PID_H_ */

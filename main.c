/*
 * main.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#define PART_TM4C123GH6PM

// Interrupts
// User inputs
// Quadrature Signals


int main(void) {
	
	// Initialise Stuff
	// Calibration

	while (1) {
		// Copy current height from ADC
		// Copy quadrature signals, calculate new location

		// Update target height
		// Update target yaw

	// Initialise Stuff
	// Calibration

	while (1) {
		// Copy current height from ADC
		// Copy quadrature signals, calculate new location

		// Update target height
		// Update target yaw


		// Update the new PWM to send to Head Rotor
		// Update the PWM to send to the Tail Rotor

		// Update Display (skip updating every X ticks instead)
		// Send status to UART (skip updating every X ticks)
	}

	return 0;
}

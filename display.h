/*
 * display.h
 * Provide Initilisation function for OLED display
 *
 * Authors: Liam Diprose, Aden Durney
 */


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stdint.h"
#include "OrbitOLEDInterface.h"
#include "utils/ustdlib.h"	// usprintf()

// Initialise display
void display_init(void);

// Update altitude reading on OLED display
void display_update(int32_t current_alt, int32_t target_alt,
		    int32_t current_yaw, int32_t target_yaw);

#endif				/* DISPLAY_H_ */

/*
 * button.h
 *
 *  Created on: Mar 23, 2017
 *      Author: ldi30
 */

#ifndef BUTTON_H_
#define BUTTON_H_

typedef struct button_s {
	uint32_t gpio_base;
	uint32_t gpio_pin;
	int last_pressed;
} Button;

// Create a button struct
Button button_init(uint32_t gpio_base, uint32_t gpio_pin) {

// Return true if the button was pressed. False if it is a bounce.
bool button_pressed(Button button, int time);

#endif /* BUTTON_H_ */

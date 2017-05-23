/*
 * button.h
 *
 *  Created on: Mar 23, 2017
 *      Author: ldi30
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"

#define BUTTON_STATE_PRESSED 1
#define BUTTON_STATE_RELEASED 0

#define BUTTON_CHECK_MS 10		  // TODO: Change
#define BUTTON_PRESS_THRES_MS 200
#define BUTTON_COUNT_START 2
#define BUTTON_NUM 4


typedef struct button_s {
	uint32_t gpio_base;
	uint32_t gpio_pin;
	char new_state;  // What is the counter trying to validate
	uint32_t count;
	char state;     // Current state of button
	char updated;      // Has current state been read yet?
} Button;


Button buttons[BUTTON_NUM];


// Check all the buttons in the global list buttons, handling their state changes
void button_check(void);

// Create a button struct
Button button_init(uint32_t gpio_base, uint32_t gpio_pin);

// Return true if button has updated and turned on, false elsewise
bool button_read(Button* btn);


#endif /* BUTTON_H_ */

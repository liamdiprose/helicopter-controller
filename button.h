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

#define BUTTON_DEBOUNCE_TIMEOUT 200
#define SEQ_READS_UP 3
#define SEQ_READS_DOWN 5

typedef struct button_s {
	uint32_t gpio_base;
	uint32_t gpio_pin;
	uint32_t num_seq_reads;
	bool butt_state;
	bool task_state;
} Button;

Button buttons[2];

// Check all the buttons in the global list buttons, handling their state changes
void button_check(void);

// Create a button struct
Button button_init(uint32_t gpio_base, uint32_t gpio_pin);



#endif /* BUTTON_H_ */

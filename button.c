
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "button.h"

#include "driverlib/gpio.h"



Button BUTTONS[BUTTON_NUM];

// Create a button struct
Button button_init(uint32_t gpio_base, uint32_t gpio_pin) {

	// Configure input to be GPIO
	GPIOPinTypeGPIOInput(gpio_base, gpio_pin);
	// Configure pin to be pull up (button will short to ground when pressed)
	GPIOPadConfigSet(gpio_base, gpio_pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	Button new_button = {
			.gpio_base = gpio_base,
			.gpio_pin = gpio_pin,
			.new_state = false,
			.count = BUTTON_COUNT_START,
			.state = false,
			.updated = false
	};

	return new_button;
}

void button_check(void) {
	uint32_t i;
	Button* current_button;
	bool current_pin_state;

	for (i = 0; i < BUTTON_NUM; i++) {
		current_button = &BUTTONS[i];
		// Ignore button if it is nothing
		if (current_button == NULL) {
			continue;
		}
		current_pin_state = GPIOPinRead(current_button->gpio_base, current_button->gpio_pin) > 1;
		if (current_pin_state == BUTTON_STATE_PRESSED) {
			if (current_button->new_state == BUTTON_STATE_PRESSED) {
				if (current_button->count > 0) {
					current_button->count--;
				}
			} else { 
				current_button->new_state = BUTTON_STATE_PRESSED;
				current_button->count = BUTTON_COUNT_START;
			}
		} else {
			if (current_button->new_state == BUTTON_STATE_RELEASED) {
				if (current_button->count > 0) {
					current_button->count--;
				}
			} else { 
				current_button->new_state = BUTTON_STATE_RELEASED;
				current_button->count = BUTTON_COUNT_START;
			}
		}
		// If the count expires, the new_state is accepted, if it has changed
		if (current_button->count == 0 && (current_button->state != current_button->new_state)) {
			current_button->state = current_button->new_state;
			current_button->updated = true;
		}
	}
}

// ISR
// void button_check(void) {
// 	uint32_t button_index;
// 	 for (button_index = 0; button_index < 2; button_index++) {
// 		uint32_t pins = GPIOPinRead(buttons[button_index].gpio_base, buttons[button_index].gpio_pin);
// 		if (READ_BIT(pins, 0) == 1 && buttons[button_index].butt_state == false)  {
// 			if (buttons[button_index].num_seq_reads == SEQ_READS_UP) {
// 				buttons[button_index].num_seq_reads = 0;
// 				buttons[button_index].butt_state = true;
// 				buttons[button_index].task_state = true;
// 			} else  {
// 				buttons[button_index].num_seq_reads++;
// 			}
// 		} else if (READ_BIT(pins, 0) == 1 && buttons[button_index].butt_state == true)  {
// 			if (buttons[button_index].num_seq_reads == SEQ_READS_DOWN) {
// 				buttons[button_index].num_seq_reads = 0;
// 				buttons[button_index].butt_state = false;
// 				buttons[button_index].task_state = true;
// 			} else {
// 				buttons[button_index].num_seq_reads++;
// 			}
// 		} else if (READ_BIT(pins, 0) == 0 && buttons[button_index].butt_state == false) {
// 			buttons[button_index].num_seq_reads = 0;
// 		} else if (READ_BIT(pins, 0) == 1 && buttons[button_index].butt_state == true) {
// 			buttons[button_index].num_seq_reads = 0;
// 		}
// 	 }
// }


bool button_read(Button* btn) {
	if (btn->updated) {
		btn->updated = false;
		return btn->state;
	}
	return false;
}


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "button.h"

#include "driverlib/gpio.h"

#define READ_BIT(bits, num) ((bits >> num) & 1)

// ISR
void button_check(void) {
	uint32_t button_index;
	 for (button_index = 0; button_index < 2; button_index++) {
		uint32_t pins = GPIOPinRead(buttons[button_index].gpio_base, buttons[button_index].gpio_pin);
		if (READ_BIT(pins, 0) == 1 && buttons[button_index].butt_state == false)  {
			if (buttons[button_index].num_seq_reads == SEQ_READS_UP) {
				buttons[button_index].num_seq_reads = 0;
				buttons[button_index].butt_state = true;
				buttons[button_index].task_state = true;
			} else  {
				buttons[button_index].num_seq_reads++;
			}
		} else if (READ_BIT(pins, 0) == 1 && buttons[button_index].butt_state == true)  {
			if (buttons[button_index].num_seq_reads == SEQ_READS_DOWN) {
				buttons[button_index].num_seq_reads = 0;
				buttons[button_index].butt_state = false;
				buttons[button_index].task_state = true;
			} else {
				buttons[button_index].num_seq_reads++;
			}
		} else if (READ_BIT(pins, 0) == 0 && buttons[button_index].butt_state == false) {
			buttons[button_index].num_seq_reads = 0;
		} else if (READ_BIT(pins, 0) == 1 && buttons[button_index].butt_state == true) {
			buttons[button_index].num_seq_reads = 0;
		}
	 }
}


// Create a button struct
Button button_init(uint32_t gpio_base, uint32_t gpio_pin) {

	// Configure input to be GPIO
	GPIOPinTypeGPIOInput(gpio_base, gpio_pin);
	// Configure pin to be pull up (button will short to ground when pressed)
	GPIOPadConfigSet(gpio_base, gpio_pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	Button new_button = {gpio_base, gpio_pin, 0, false, false};

	return new_button;
}

bool button_pressed(Button* btn) {
	if( btn->task_state ) {
		btn->task_state = false;
		return true;
	}
	return false;
}


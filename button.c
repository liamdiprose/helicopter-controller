
#include "button.h"

Button buttons[BUTTON_NUM];
uint8_t next_button_index = 0;

// Create a button, and adds it to check list
Button* button_init(uint32_t gpio_periph, uint32_t gpio_base, uint32_t gpio_pin) {
    SysCtlPeripheralEnable(gpio_periph);
    // Wait for peripheral to be ready
    while(SysCtlPeripheralReady(gpio_periph));

	SysTickPeriodSet(SysCtlClockGet()/100);
	SysTickEnable();
	SysTickIntEnable();
	SysTickIntRegister(button_check);
    
	// Configure pin to be pull down (button will be high when pressed)
	GPIOPadConfigSet(gpio_base, gpio_pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

	// Configure input to be GPIO
	GPIOPinTypeGPIOInput(gpio_base, gpio_pin);

    
	buttons[next_button_index] = {gpio_base, gpio_pin, false, BUTTON_COUNT_START, false, false};

    Button* new_button_ptr = &buttons[next_button_index];
    next_button_index++;

	return new_button_ptr;
}


void button_check_routine(void) {
	uint32_t i;
	Button* current_button;
	bool current_pin_state;

	for (i = 0; i < BUTTON_NUM; i++) {
		current_button = &buttons[i];
		// Ignore button if it is nothing
		if (current_button == NULL) {
			continue;
		}
		current_pin_state = GPIOPinRead(current_button->gpio_base, current_button->gpio_pin) > 0;
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
			current_button->updated = 1;
		}
	}
}


ButtonStatus button_read(Button* btn) {
	if (btn->updated) {
            btn->updated = false;
            if (btn->state) {
                    //ON_EVENT, ON, OFF_EVENT, OFF
                    return ON_EVENT;
            } else {
                    return OFF_EVENT;
            }
    } else {
            if (btn->state) {
                    return ON;
            } else {
                    return OFF;
            }
	}
	return btn->state;
}


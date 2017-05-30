
#include "button.h"
#include "debug.h"

Button buttons[BUTTON_NUM];
uint8_t next_button_index = 0;

// Forward Declearation of ISR's
void button_check_routine(void);


// Create a button, and adds it to check list
Button* button_init(uint32_t gpio_periph, uint32_t gpio_base, uint32_t gpio_pin, ButtonPullDirection pull_direction) {

	SysCtlPeripheralEnable(gpio_periph);
    // Wait for peripheral to be ready
    while(!SysCtlPeripheralReady(gpio_periph));
	// Port F is already enabled, as it had to be unlocked
	if (gpio_periph == SYSCTL_PERIPH_GPIOF) {
		GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
		GPIO_PORTF_CR_R |= GPIO_PIN_0; //PF0 unlocked
		GPIO_PORTF_LOCK_R = GPIO_LOCK_M;
	}


    
	// Configure input to be GPIO
	GPIOPinTypeGPIOInput(gpio_base, gpio_pin);


	Button new_button;
	// Configure Pad to pull high when button disconnected
	switch ( pull_direction ) {
		case PULL_DOWN: {
				GPIOPadConfigSet(gpio_base, gpio_pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
				new_button = (Button) {gpio_base, gpio_pin, pull_direction, 0, BUTTON_STATE_RELEASED, 0};
		}; break;
		case PULL_UP: {
			GPIOPadConfigSet(gpio_base, gpio_pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
			new_button = (Button) {gpio_base, gpio_pin, pull_direction, BUTTON_COUNT_START, BUTTON_STATE_PRESSED, 0};
		}; break;
		case NONE: {
			GPIOPadConfigSet(gpio_base, gpio_pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
			new_button = (Button) {gpio_base, gpio_pin, pull_direction, BUTTON_COUNT_START, BUTTON_STATE_PRESSED, 0};
		};break;
	}


	buttons[next_button_index] = new_button;

    Button* new_button_ptr = &buttons[next_button_index];
    next_button_index++;

	return new_button_ptr;
}


void button_check_routine(void) {

	Button* current_button;
	bool current_pin_state;

	uint32_t i;
	for (i = 0; i < BUTTON_NUM; i++) {
		current_button = &buttons[i];

		if (current_button == NULL) {
			// Stop Loop when no more buttons in array
			break;
		}
		current_pin_state = GPIOPinRead(current_button->gpio_base, current_button->gpio_pin) > 0;
		if (current_pin_state == BUTTON_STATE_PRESSED) {
			if (current_button->count < BUTTON_COUNT_START) {
				current_button->count++;
			}
		} else {
			if (current_button->count > 0) {
				current_button->count--;
			}
		}

		// If count has reached zero, and current state is pushed, update to released
		if (current_button->count == 0 && current_button->state == BUTTON_STATE_PRESSED) {
			current_button->state = BUTTON_STATE_RELEASED;
			current_button->updated = 1;
		} else if (current_button->count == BUTTON_COUNT_START && current_button->state == BUTTON_STATE_RELEASED) {
			current_button->state = BUTTON_STATE_PRESSED;
			current_button->updated = 1;
		}
	}
}

// Take logic state and updated status of button, and return ButtonStatus enum
ButtonStatus button_status(Button* btn) {
	ButtonStatus status;

	// Convert logic state to pressed state
	if (btn->pull_dir == PULL_UP) {
		// Logic 1 means button was pressed
		status = btn->state ? PRESSED : RELEASED;
	} else {
		// Logic 0 means button was pressed
		status = btn->state ? RELEASED : PRESSED;
	}

	//  Convert pressed state to updated state
	if (btn->updated) {
		status = (status == PRESSED) ? PRESS_EVENT : RELEASE_EVENT;
		btn->updated = false;  // Prevent button press being acted apon twice
	}

	return status;
}



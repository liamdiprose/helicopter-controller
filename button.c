
#include <stdio.h>
#include <stdint.h>

#include "button.h"

#include "driverlib/gpio.h"

// Create a button struct
Button button_init(uint32_t gpio_base, uint32_t gpio_pin) {

	// Configure input to be GPIO
	GPIOPinTypeGPIOInput(gpio_base, gpio_pin);
	// Configure pin to be pull up (button will short to ground when pressed)
	GPIOPadConfigSet(gpio_base, gpio_pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	Button new_button = {gpio_base, gpio_pin, 0};

	return new_button;
}

// Return true if the button was pressed. False if it is a bounce.
bool button_pressed(Button button) {
		uint32_t clock = SysCtlClockGet();
		return button.last_pressed + clock * BUTTON_DEBOUNCE_TIMEOUT / 4000 > clock;
}

// Create a button struct
Button button_init(uint32_t gpio_base, uint32_t gpio_pin) {
		return {gpio_base, gpio_pin, 0}
}

// Return true if the button was pressed. False if it is a bounce.
bool button_pressed(Button button, int time) {
		return button.last_pressed + BUTTON_DEBOUNCE_TIMEOUT > time;
}


#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"

#define BUTTON_STATE_PRESSED 1
#define BUTTON_STATE_RELEASED 0

#define BUTTON_CHECK_MS 10		  // TODO: Change
#define BUTTON_PRESS_THRES_MS 200
#define BUTTON_COUNT_START 2

// Maximum amount of buttons to poll
#define BUTTON_NUM 5

typedef enum {
        PRESS_EVENT, PRESSED, RELEASED, RELEASE_EVENT
} ButtonStatus;

typedef enum {
	PULL_UP, PULL_DOWN, NONE
} ButtonPullDirection;

typedef struct button_s {
	uint32_t gpio_base;
	uint32_t gpio_pin;
	ButtonPullDirection pull_dir;  // What is the counter trying to validate
	uint32_t count;
	uint8_t state;     // Current state of button
	char updated;      // Has current state been read yet?
} Button;





void button_check_routine(void);

// Create a button struct
Button* button_init(uint32_t gpio_periph, uint32_t gpio_base, uint32_t gpio_pin, ButtonPullDirection pull_direction);


// Return true if button has updated and turned on, false elsewise
ButtonStatus button_read(Button* btn);


#endif /* BUTTON_H_ */

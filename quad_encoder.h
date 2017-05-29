#ifndef QUAD_ENCODER_H_
#define QUAD_ENCODER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

#define QUAD_GPIO_PERIPH SYSCTL_PERIPH_GPIOB

#define QUAD_GPIO_BASE GPIO_PORTB_BASE
#define QUAD_GPIO_PIN_0 GPIO_PIN_0
//#define QUAD_GPIO_PIN_0 GPIO_PIN_5
#define QUAD_GPIO_PIN_1 GPIO_PIN_1
#define QUAD_GPIO_PINS (QUAD_GPIO_PIN_0 | QUAD_GPIO_PIN_1)

#define QUAD_GPIO_INT_PIN_0 GPIO_INT_PIN_0
#define QUAD_GPIO_INT_PIN_1 GPIO_INT_PIN_1
//#define QUAD_GPIO_INT_PIN_1 GPIO_INT_PIN_5
#define QUAD_GPIO_INT_PINS (QUAD_GPIO_INT_PIN_0 | QUAD_GPIO_INT_PIN_1)


//initialise the quadrature
void quad_init(void);

// give the current yaw (quadrature encoded) position in degrees
int32_t quad_get_degrees();


#endif /* QUAD_ENCODER_H_ */



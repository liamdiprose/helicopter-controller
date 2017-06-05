/*
 * uart.h
 * Provide initialiation for UART serial output.
 *
 *  Created on: Mar 22, 2017
 *      Authors: Aden Durney, Liam Diprose
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>  // Allow print to take multiple parameters (printf like functionality)

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

#include "utils/ustdlib.h"
#include "driverlib/pin_map.h"



#define UART_BAUD_RATE			9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX

// Initialise UART output
void uart_init(void);

void uart_print(char* message);


#endif /* UART_H_ */

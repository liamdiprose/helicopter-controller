
#include "uart.h"



void uart_init(void) {
	SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
	SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(UART_USB_GPIO_BASE,  UART_USB_GPIO_PINS);

	UARTStdioConfig(0, UART_BAUD_RATE, SysCtlClockGet());
}

// TODO: use <stdarg.h> to support undefined amount of parameters,
// then pass it to usprintf?
void uart_print(char* message) {
//	while(*message) {
//		UARTCharPut(UART_USB_BASE, *message);
//		message++;
//	}
}


#include "uart.h"

void uart_init(void) {
	SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
	SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);

	GPIOPinTypeUART(UART_USB_GPIO_BASE,  UART_USB_GPIO_PINS);

	UARTConfigSetExpClk(UART_USB_GPIO_BASE, SysCtlClockGet(), UART_BAUD_RATE, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTFIFOEnable(UART_USB_GPIO_BASE);
	UARTEnable(UART_USB_GPIO_BASE);
}

// TODO: use <stdarg.h> to support undefined amount of parameters,
// then pass it to usprintf?
void uart_print(char* message) {
	while(*message) {
		UARTCharPut(UART_USB_GPIO_BASE, *message);
		message++;
	}
}


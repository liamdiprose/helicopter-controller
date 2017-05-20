#include "uart.h"

int uart_init(void) {
	SysCtlPeripheralEnable(UART_USB_PERIPH_BASE);
	SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);

	GPIOPinTypeUART(UART_USB_GPIO_BASE,  UART_GPIO_PIN_RX | UART_GPIO_TX);

	UARTConfigSetExpClock(UART_USB_GPIO_BASE, SysCtlClockGet(), UART_BAUD_RATE, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
	UARTFIFOEnable(UART_USB_GPIO_BASE);
	UARTEnable(UART_USB_GPIO_BASE);
}

// TODO: use <stdarg.h> to support undefined amount of parameters,
// then pass it to usprintf?
void uart_print(char* message) {
	while(*message) {
		UARTCharPut(*message);
		message++;
	}
}


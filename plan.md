# Pseudocode of highest level

## main function

Initilise variables
Initialise interrupts
	* Push buttons: Adjust respective target if hasn't been pressed in last X ticks
	* Yaw quadrature signals: ** Logic to determine what direction **. Update a yaw variable. 

while 1:
	Get ADC value for height
	Average it with previous values

	Feed current height and target height into PID controller. 
	Write new value to PWM.

	Feed current yaw and target yaw into PID controller.
	Write new value to PWM.

	Print shit to OLED
	Report CSV data to UART

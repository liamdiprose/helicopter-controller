/*
 * timer.c
 *
 *  Created on: May 25, 2017
 *      Author: ldi30
 */

#include "timer.h"

uint32_t millis = 0;
uint32_t lap = 0;

void timer_ms_routine(void) {
	  TimerIntClear(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
	  millis++;
}


void timer_init(void) {

	SysCtlPeripheralEnable(TIMER_PERIPH);
	while(!SysCtlPeripheralReady(TIMER_PERIPH));

	TimerConfigure(TIMER_BASE, TIMER_CFG_PERIODIC);
	//TimerPrescaleSet(TIMER_BASE, TIMER_BOTH, 255);
	uint32_t period = SysCtlClockGet()/1000;  // 1ms
	TimerLoadSet(TIMER_BASE, TIMER_A, period-1);

	TimerIntRegister(TIMER_BASE, TIMER_A, timer_ms_routine);
	TimerIntEnable(TIMER_BASE, TIMER_TIMA_TIMEOUT);

	TimerEnable(TIMER_BASE, TIMER_A);
}

// Get amount of milliseconds since the timer was cleared.
uint32_t timer_get_millis(void) {

	return millis;

}

// Set the timer to 0
void timer_clear(void) {
	millis = 0;
}


// Set a marker for a new lap
void timer_set_lap(void) {
	lap = millis;
}

// Get time since last lap
uint32_t timer_get_lap(void) {
	return millis - lap;
}

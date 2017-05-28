/*
 * timer.c
 *
 *  Created on: May 25, 2017
 *      Author: ldi30
 */

#include "timer.h"

#define MAX_UINT32 0x7FFFFFFF

uint32_t g_millis = 0;
uint32_t g_lap = 0;

void timer_ms_routine(void) {
	  TimerIntClear(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
	  g_millis++;
}


void timer_init(void) {


	SysCtlPeripheralEnable(TIMER_PERIPH);
	while(!SysCtlPeripheralReady(TIMER_PERIPH));

	TimerConfigure(TIMER_BASE, TIMER_CFG_PERIODIC_UP);
	TimerClockSourceSet(TIMER_BASE, TIMER_CLOCK_SYSTEM);
	//TimerPrescaleSet(TIMER_BASE, TIMER_A, 255);

	TimerLoadSet(TIMER_BASE, TIMER_A, MAX_UINT32);

	TimerEnable(TIMER_BASE, TIMER_A);
}

// Get amount of milliseconds since the timer was cleared.
uint32_t timer_get_millis(void) {
	return TimerValueGet(TIMER_BASE, TIMER_A) / (SysCtlClockGet()/1000000);
}

// Set the timer to 0
void timer_clear(void) {
	//TimerValueSet(TIMER_BASE, TIMER_A, 0);
}

// Set a marker for a new lap
void timer_set_lap(void) {
	g_lap = timer_get_millis();
}

// Get time since last lap
uint32_t timer_get_lap(void) {
	return timer_get_millis() - g_lap;
}

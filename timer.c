#include <math.h>

#include "timer.h"
#include "debug.h"

#define TIMER_TIMEOUT_VAL 0xFFFFFFFF	// Maximum 32 Unsigned number

// Save how many times timer has wrapped around to prevent wrap-around errors
uint32_t g_loops = 0;

uint64_t g_saves[TIMER_NUM_RECORDS] = { 0 };

void timer_timeout_routine(void)
{
	TimerIntClear(TIMER_BASE, TIMER_TIMA_TIMEOUT);
	g_loops++;
}

void timer_init(void)
{

	SysCtlPeripheralEnable(TIMER_PERIPH);
	while (!SysCtlPeripheralReady(TIMER_PERIPH)) ;

	TimerConfigure(TIMER_BASE, TIMER_CFG_PERIODIC_UP);
	TimerClockSourceSet(TIMER_BASE, TIMER_CLOCK_SYSTEM);
	//TimerPrescaleSet(TIMER_BASE, TIMER_A, 255);

	TimerIntRegister(TIMER_BASE, TIMER_A, timer_timeout_routine);
	TimerLoadSet(TIMER_BASE, TIMER_A, TIMER_TIMEOUT_VAL);

	TimerIntEnable(TIMER_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER_BASE, TIMER_A);
}

// Get amount of milliseconds since the timer was cleared.
uint64_t timer_get_micros(void)
{
	uint32_t period = (SysCtlClockGet() / ONE_MICROSECOND);
	return (uint64_t) ((uint64_t) TimerValueGet(TIMER_BASE, TIMER_A) +
			   (uint64_t) g_loops * TIMER_TIMEOUT_VAL) / period;
}

// Helper function that returns time in SI units
float timer_get_seconds(void)
{
	return (float)timer_get_micros() / ONE_MICROSECOND;
}

// Record the time to buffer at index
void timer_record(uint8_t index)
{
	g_saves[index] = timer_get_micros();
}

// Set a marker for a new lap
uint64_t timer_micros_since(uint8_t index)
{
	return timer_get_micros() - g_saves[index];
}

// Return seconds since index happened
float timer_seconds_since(uint8_t index)
{
	return roundf((float)timer_micros_since(index) / ONE_MICROSECOND *
		      1000) / 1000;
}

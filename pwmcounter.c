/*
 * pwmcounter.c
 *
 *  Created on: Apr 2, 2017
 *      Author: ldi30
 */

#include "pwmcounter.h"

void pwmcounter_isr(void) {
	TimerIntClear(PWMCOUNTER_BASE, TIMER_TIMB_TIMEOUT);
	pwmcounter_count = TimerValueGet(PWMCOUNTER_BASE, TIMER_A);
	TimerLoadSet(PWMCOUNTER_BASE, TIMER_A, PWMCOUNTER_COUNT_START);
}


// Return pointer to last known frequency
void pwmcounter_init() {

	uint32_t clock = SysCtlClockGet();

	//TimerConfigure(TIMER2_BASE, TIMER_CFG_B_ONE_SHOT);
	TimerConfigure(PWMCOUNTER_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT | TIMER_CFG_B_PERIODIC));
	TimerPrescaleSet(PWMCOUNTER_BASE, TIMER_B, 255);  //

	TimerLoadSet(PWMCOUNTER_BASE, TIMER_A, PWMCOUNTER_COUNT_START);
	TimerLoadSet(PWMCOUNTER_BASE, TIMER_B, 65534);
	TimerControlEvent(PWMCOUNTER_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
	TimerIntRegister(PWMCOUNTER_BASE, TIMER_B, pwmcounter_isr);
	TimerIntEnable(PWMCOUNTER_BASE, TIMER_TIMB_TIMEOUT);
	IntEnable(INT_TIMER3B);

	// Start both timers
	TimerEnable(PWMCOUNTER_BASE, TIMER_BOTH);

}

// Get last recorded frequency
uint32_t pwmcounter_freq_get(void) {
	// TODO: Adjust return frequency
	return 5 * (PWMCOUNTER_COUNT_START - pwmcounter_count);
}


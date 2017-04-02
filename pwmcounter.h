/*
 * pwmcounter.h
 *
 *  Created on: Apr 2, 2017
 *      Author: ldi30
 */

#ifndef PWMCOUNTER_H_
#define PWMCOUNTER_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"


//#define PWMCOUNTER_BASE TIMER2_BASE
#define PWMCOUNTER_BASE TIMER3_BASE

// The counter counts down, but from where?...
#define PWMCOUNTER_COUNT_START 1000


// Clear some space for pwm counter
uint32_t pwmcounter_count;

void pwmcounter_isr(void);

// Return pointer to last known frequency
void pwmcounter_init();

uint32_t pwmcounter_freq_get(void);

#endif /* PWMCOUNTER_H_ */

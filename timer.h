/*
 * timer.h
 *
 *  Created on: May 25, 2017
 *      Author: ldi30
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#define TIMER_PERIPH SYSCTL_PERIPH_TIMER5
#define TIMER_BASE TIMER5_BASE

void timer_init(void);

// Get amount of milliseconds since the timer was cleared.
uint32_t timer_get_millis(void);

// Set the timer to 0
void timer_clear(void);

// Set a marker for a new lap
void timer_set_lap(void);

// Get time since last lap
uint32_t timer_get_lap(void);

#endif /* TIMER_H_ */

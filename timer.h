/*
 * timer.h
 * Provides functions for measureing time of program, and time periods. 
 * Accounts for timer wrap around.
 *
 * Authors: Liam Diprose, Aden Durney
 */


#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#define TIMER_TIMEOUT 0x7FFFFFFF
#define TIMER_NUM_RECORDS 5


#define TIMER_PERIPH SYSCTL_PERIPH_TIMER5
#define TIMER_BASE TIMER5_BASE

#define ONE_MICROSECOND 1000000

void timer_init(void);

float timer_get_seconds(void);
// Get amount of milliseconds since the timer was cleared.
uint64_t timer_get_micros(void);


// Record time, for measureing period later on
void timer_record(uint8_t index);

// Return time period since `time_record(index)` call
uint64_t timer_micros_since(uint8_t index);

float timer_seconds_since(uint8_t index);

#endif /* TIMER_H_ */

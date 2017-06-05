/*
 * debug.h
 *  Provide Debugging log macro for serial debugging
 *
 *  Authors: Liam Diprose, Aden Durney
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include "utils/uartstdio.h"

#define DEBUGGING_MODE 1
#define DEBUGGING_TAG "[D] "

#if DEBUGGING_MODE
#define DEBUG(f,...) UARTprintf(DEBUGGING_TAG);UARTprintf(f, ##__VA_ARGS__);UARTprintf("\r\n")
#else
#define DEBUG(...) do {} while (0)	// Do Nothing
#endif

#endif				/* DEBUG_H_ */

/*
 * debug.h
 *  Provide Debugging log macro
 *  Created on: May 21, 2017
 *      Author: ldi30
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#ifndef DEBUGGING_MODE
	#define DEBUGGING_MODE 0
#endif

#if DEBUGGING_MODE
    #define DEBUG(f,...) uart_print(f)  // TODO: Support mutliple variables with __VA_ARGS__
#else
    #define DEBUG(...) do {} while (0) // Do Nothing
#endif

#endif /* DEBUG_H_ */

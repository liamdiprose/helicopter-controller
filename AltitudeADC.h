/*
 * AltitudeADC.h
 *
 *  Created on: May 7, 2017
 *      Author: and22
 */

#ifndef ALTITUDEADC_H_
#define ALTITUDEADC_H_


#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/ustdlib.h"


#define OLD_ALT_VALUE_RATIO 0.7
#define NEW_ALT_VALUE_RATIO 0.3

uint32_t altitude_hold[1];
uint32_t altitude_current;
uint32_t bottom_altitude_hold[1];
uint32_t bottom_altitude_current;
uint32_t altitude_range;
uint32_t bottom_altitude;
uint32_t top_altitude;

//initialize the peripherals and variables required for altitute
void Altitude_init(void);


//interrpt to be registered
void update_Altitude(void);

uint32_t altitude_get_percent(void);


#endif /* ALTITUDEADC_H_ */



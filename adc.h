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
#include "utils/ustdlib.h"
#include "driverlib/systick.h"


#define ALT_NEW_RATIO 0.3
#define ALT_OLD_RATIO (1 - ALT_NEW_RATIO)

#define ADC_GPIO_PERIPH SYSCTL_PERIPH_GPIOE
#define ADC_ADC_PERIPH SYSCTL_PERIPH_ADC0
#define ADC_GPIO_BASE GPIO_PORTE_BASE
#define ADC_GPIO_PIN GPIO_PIN_4
#define ADC_ADC_BASE ADC0_BASE

// Initialize the peripherals and variables required for altitute
void adc_init(void);

// Interrpt to be registered
void adc_update_routine(void);

// Get the latest ADC value in percentage 
uint32_t adc_get_percent(void);

#endif /* ALTITUDEADC_H_ */

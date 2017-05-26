/*
 * AltitudeADC.c
 *
 *  Created on: May 7, 2017
 *      Author: and22
 */
#include "adc.h"

uint32_t g_adc_current;

void adc_init(void){
	SysCtlPeripheralEnable(ADC_GPIO_PERIPH);
	SysCtlPeripheralEnable(ADC_ADC_PERIPH);

	GPIOPinTypeADC(ADC_GPIO_BASE, ADC_GPIO_PIN);

	ADCSequenceStepConfigure(ADC_ADC_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceEnable(ADC_ADC_BASE, 3);

	//uint32_t altitude_hold[0]=0;
	g_adc_current = 0;
	SysTickIntRegister(adc_update_routine);
	//uint32_t bottom_altitude_hold[0]=0;
	//uint32_t bottom_altitude_current=0;
    // TODO: Move calibration to own procedure in main.c
//	for (uint32_t i = 0; i < 8; i++) {
//		ADCProcessorTrigger(ADC0_BASE, 3);
//        // Wait until ADC is ready
//		//while(!ADCIntStatus(ADC0_BASE, 3, false));
//		//ADCIntClear(ADC0_BASE, 3);
//		ADCSequenceDataGet(ADC0_BASE, 3, bottom_altitude_hold);
//		//bottom_altitude += bottom_altitude_hold[0];
//		SysCtlDelay(22);
//	}

	//bottom_altitude= bottom_altitude/8;
	//top_altitude=bottom_altitude-1000;
	//altitude_range = bottom_altitude - top_altitude;
}

void adc_update_routine(void) {
	//ADCIntClear(ADC0_BASE, 3);

	uint32_t altitude_hold[1];

	ADCProcessorTrigger(ADC0_BASE, 3);
	//while(!ADCIntStatus(ADC0_BASE, 3, false));
	ADCSequenceDataGet(ADC0_BASE, 3, altitude_hold);

    uint32_t old_value = g_adc_current;
    uint32_t new_value = altitude_hold[0];

	g_adc_current = old_value * ALT_OLD_RATIO + new_value * ALT_NEW_RATIO;
}

// Get the latest ADC value in percentage
uint32_t adc_get_percent(void) {
        return g_adc_current;
}

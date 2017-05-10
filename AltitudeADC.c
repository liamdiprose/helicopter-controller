/*
 * AltitudeADC.c
 *
 *  Created on: May 7, 2017
 *      Author: and22
 */
#include "AltitudeADC.h"

void Altitude_init(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);

	//ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 3);
	ADCIntClear(ADC0_BASE, 3);

	//GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);

//  Ch.9 of Tiva's ADC (AIN9) is used to connect to an external waveform
    //ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);

    // Since sample sequence 3 is now configured, it must be enabled.
    //ADCSequenceEnable(ADC0_BASE, 3);

    // Clear the interrupt status flag.  This is done to make sure the interrupt flag is cleared before we sample.
    //ADCIntClear(ADC0_BASE, 3);


	altitude_hold[0]=0;
	altitude_current = 0;
	bottom_altitude_hold[0]=0;
	bottom_altitude_current=0;
	uint32_t i=0;
	for ( i=0; i<8;i++){
		ADCProcessorTrigger(ADC0_BASE, 3);
		while(!ADCIntStatus(ADC0_BASE, 3, false)) {
			//wait until the ADC is ready
		}
		ADCIntClear(ADC0_BASE, 3);
		ADCSequenceDataGet(ADC0_BASE, 3, bottom_altitude_hold);
		bottom_altitude += bottom_altitude_hold[0];
		SysCtlDelay(22);
	}

	bottom_altitude= bottom_altitude/8;
	top_altitude=bottom_altitude-1000;
	altitude_range = bottom_altitude-top_altitude;


}

void update_Altitude(void){
	ADCProcessorTrigger(ADC0_BASE, 3);
	while(!ADCIntStatus(ADC0_BASE, 3, false)) {
	}
	ADCIntClear(ADC0_BASE, 3);
	ADCSequenceDataGet(ADC0_BASE, 3, altitude_hold);
	altitude_current = (altitude_current*OLD_ALT_VALUE_RATIO)+((altitude_hold[0])*NEW_ALT_VALUE_RATIO);

}

uint32_t altitude_get_percent(void){
	return 100-(100*(altitude_current-top_altitude)/altitude_range);
}

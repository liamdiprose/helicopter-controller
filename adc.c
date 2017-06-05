#include "adc.h"

uint32_t g_adc_current;

void adc_init(void)
{
	SysCtlPeripheralEnable(ADC_GPIO_PERIPH);
	SysCtlPeripheralEnable(ADC_ADC_PERIPH);

	while (!SysCtlPeripheralReady(ADC_GPIO_PERIPH)) ;
	while (!SysCtlPeripheralReady(ADC_ADC_PERIPH)) ;
	GPIOPinTypeADC(ADC_GPIO_BASE, ADC_GPIO_PIN);

	ADCSequenceStepConfigure(ADC_ADC_BASE, 3, 0,
				 ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceEnable(ADC_ADC_BASE, 3);

	g_adc_current = 0;
}

void adc_update_routine(void)
{

	uint32_t altitude_hold[1];

	ADCProcessorTrigger(ADC0_BASE, 3);
	ADCSequenceDataGet(ADC0_BASE, 3, altitude_hold);

	uint32_t old_value = g_adc_current;
	uint32_t new_value = altitude_hold[0];

	g_adc_current = old_value * ALT_OLD_RATIO + new_value * ALT_NEW_RATIO;
}

// Get the latest ADC value in percentage
uint32_t adc_get_percent(void)
{
	return 100 * (g_adc_current / (float)4094.0);
}

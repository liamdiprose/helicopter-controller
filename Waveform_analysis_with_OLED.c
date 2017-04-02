/************************************************************************/
/*	ENCE361 laboratory																	*/
/*	Waveform_analysis_with_OLED.c - main program file for Week-4 lab	*/
/*																		*/
/************************************************************************/
/*	Author: 	Steve Weddell											*/
/*	Electrical & Computer Engineering, for Embedded Systems 1			*/
/************************************************************************/
/*  Module Description: 												*/
/*  This program uses pin PE4 as an external ADC input, AIN9, 			*/
/*	to acquire values from a periodic waveform. These values are 		*/
/*	placed in a circular buffer which uses the heap for memory storage.	*/
/*  Heap memory deallocation and program exit are completed by the user */
/*  pressing switch SW1.												*/
/*	In addition, initialisation and display functions for the OLED		*/
/*	display are included. While running, values will constantly change. */
/*	However, setting a breakpoint at the statement immediately after 	*/
/*	updating the display will show the current digitised value of the   */
/*	ADC from the input waveform.										*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/*																		*/
/*	02/03/2017(Steve Weddell): Created									*/
/*  11/03/2017 (Steve Weddell): last modified.							*/
/*																		*/
/*  Modifications:														*/
/*   1. Updated to a new version of Phil Bones' circular buffer ADT		*/
/*   2. Updated Matthew's OLED routines for character location			*/
/*	 3. Provided deallocation of heap memory on exit, although embedded */
/*	    systems rarely exit, and to do so typically suggests a problem!	*/
/************************************************************************/

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h> //needed for sprintf

#include	"OrbitOLEDInterface.h"  // for the Orbit OLED display

/* Standard TivaWare library includes */
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"

#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

#include "utils/ustdlib.h"  // for usprintf
#include "circBufT.h"		// Phil's circular buffer ADT

/* -------------------------------------------------------------- */
/*				General Type Definitions						  */
/* -------------------------------------------------------------- */

#define ONE_MILLISECOND 3000  	// for SysCtrlDelay()
#define ONE_MS 80000			// for SysTick
#define BUF_SIZE 100			// size of circular buffer

#define SWITCH_SW1_INACTIVE (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == GPIO_PIN_4)


/* ------------------------------------------------------------ */
/*				Local Type Definitions							*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/*				Static Variables								*/
/* ------------------------------------------------------------ */
static circBuf_t g_inBuffer;	// buffer of size BUF_SIZE integers

/* ------------------------------------------------------------ */
/*				Forward Declarations							*/
/* ------------------------------------------------------------ */
void DeviceInit();
void SampleStoreDisplay();
void SysTick_IntHandler(void);  // SysTick interrupt handler

/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/***	main()
**
**	Parameters:
**		none
**
**	Return Value:
**		deallocation of heap memory and exit program
**
**	Errors:
**		As of 11 March 2017, none found
**
**	Description:
**		Execution of the main loop is conditional of switch SW1
**		 - ADC is sampled, stored, and sampled values are displayed on the
**		   OLED display
**		 - a 1 ms delay is implemented
*/
int main() {

	char mainMessage[ 24 ]; // character array for OLED display

	DeviceInit();

	while (SWITCH_SW1_INACTIVE) // Read pin PF4 of the Tiva board (Switch SW1)
	{
		SampleStoreDisplay();	// This function is not a good example of modularsation!
		SysCtlDelay(SysCtlClockGet()/ONE_MILLISECOND);  // delay, supposedly for 1 ms

	}
	freeCircBuf (&g_inBuffer);  // free memory reserved on the heap
	usprintf(mainMessage, "Deallocate heap\0");
	OLEDStringDraw(mainMessage, 0, 2);			// send a message to the display
	usprintf(mainMessage, "Exit program\0");
	OLEDStringDraw(mainMessage, 0, 3);			// send a message to the display
	return 0;
} // end of program


/* ------------------------------------------------------------ */
/***	DeviceInit
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Description:
**		Various GPIO ports, the ADC (Ch 9), and OLED peripherals are initalised,
**		and memory is allocated on the heap of size, BUF_SIZE.
**
*/
void DeviceInit(void)
{
	/*
	 * First, Set Up the Clock.
	 * Main OSC				-> SYSCTL_OSC_MAIN
	 * Runs off 16MHz clock -> SYSCTL_XTAL_16MHZ
	 * Use PLL				-> SYSCTL_USE_PLL
	 * Divide by 2.5	-> SYSCTL_SYSDIV_2_5
	 * Note, there is an internal divide by 2, which is a default for this configuration.
	 */
	SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_2_5);

	// set up SysTick for later use
	//
	SysTickIntRegister(SysTick_IntHandler);

	SysTickPeriodSet(ONE_MS);  // set the period; of course, this can be increased!
	IntMasterEnable();		   //
	SysTickIntEnable();
	SysTickEnable();

	/*
	 * Enable all GPIO Ports used by the Tiva kit, e.g., SW1,
	 * the Orbit OLED, and possibly other ORBIT functions
	 */
	SysCtlPeripheralEnable(	SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOE);
	SysCtlPeripheralEnable( SYSCTL_PERIPH_GPIOF);  // for SW1 switch

	 /* Enable ADC Peripheral
	  * use PE4 AIN9
	 */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	// Use Pin-4 of Port-E (PE4) for an external waveform connection to connect to Tiva's AIN9
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_4);

	ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

//  Ch.9 of Tiva's ADC (AIN9) is used to connect to an external waveform
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE | ADC_CTL_END);

    // Since sample sequence 3 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC0_BASE, 3);

    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    //
    ADCIntClear(ADC0_BASE, 3);

    // Initialise the OLED display.
    //
    OLEDInitialise();

	// make pushbutton SW1 on pin PF4 an input
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);	// make pushbutton SW1 on pin PF4 an input

	// set data direction register as output
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_DIR_MODE_IN);

    // enable F4's pullup for SW1 push-button, the drive strength won't affect the input
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    //
    // Initialise the circular buffer which allocates memory on the heap
    //
    initCircBuf(&g_inBuffer, BUF_SIZE);
}


// This routine is for students to test and experiment with SysTick...
//
void SysTick_IntHandler(void)
{

}

/* ------------------------------------------------------------ */
/***	SampleStoreDisplay()
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Description:
**		- initialise local variables
**		- acquire an ADC sample from AIN9
**		- display the value using the OLED display and exit message.
**	Note:
**		It should be obvious that improvements can be made to this routine
*/
void SampleStoreDisplay()
{
	uint32_t 		uiValue = 0; // a single ADC value
	uint32_t		ulAIN2[1];   // The ADC sequencer uses a FIFO buffer

	char cMessage[ 24 ];		 // local character array for display strings

	/*
	 * Initiate ADC Conversion
	 */
	ADCProcessorTrigger(ADC0_BASE, 3);

	while(!ADCIntStatus(ADC0_BASE, 3, false));

	ADCSequenceDataGet(ADC0_BASE, 3, ulAIN2);

	uiValue =  ulAIN2[0];		// select a single value from the FIFO buffer

	writeCircBuf (&g_inBuffer, uiValue);  // write ADC value to Phil's circular buffer

	usprintf(cMessage, "AIN9 value: %4d", uiValue);  	// format a message for the display
	OLEDStringDraw(cMessage, 0, 0);						// send a message to Line 0 of the display

	usprintf(cMessage, "Press SW1 to end");  // format a message for the display
	OLEDStringDraw(cMessage, 0, 3);			// send a message to Line 3 of the display
}

/******************************************************************/

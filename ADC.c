/* Name:  Amelia Smith
 * Assignment:	Engs 28 26F, Lab 5
 *
 * Program name:	ADC.c
 * Date created:	10.18.26
 * Description:	Defines? functions to initialize and configure the ADC, select input channel, set resolution, and read ADC values.
 *
 * I/O pins: 	PA: 0, 1, 2, 4, 5 (Potential analog inputs for temp sensor)
 * 				PB: 1 (Potential analog input for temp sensor)
 * 				PB: 8, 9 (I2C and alternate-function pins for seven-segment display)
 *
 */

/* INCLUDE FILES */
#include <stdio.h>
#include "ES28.h"
#include "uart.h"

void adc_init(void) {
	// Enable clock access to GPIOA and GPIOB
	RCC->IOPENR |=  RCC_IOPENR_GPIOAEN;	// Enable clock access to GPIOA
	RCC->IOPENR |=  RCC_IOPENR_GPIOBEN;	// Enable clock access to GPIOA

	// Configure analog input pins
	// Clear bits for each GPIOA pin before setting
	GPIOA->MODER &= ~(
			GPIO_MODER_MODE0_Msk  |  //Set PA0 to analog (A0)
			GPIO_MODER_MODE1_Msk  |  //Set PA1 to analog (A1)
			GPIO_MODER_MODE4_Msk  |  //Set PA2 to analog (A2)
			GPIO_MODER_MODE11_Msk |  //Set PA4 to analog (A4)
			GPIO_MODER_MODE12_Msk    //Set PA5 to analog (A5)
		);

	// Set selected GPIOB pin to analog
	GPIOA->MODER |= (
			(GPIO_ANALOG << GPIO_MODER_MODE0_Pos)  | //Set PA0 to analog (A0)
			(GPIO_ANALOG << GPIO_MODER_MODE1_Pos)  | //Set PA1 to analog (A1)
			(GPIO_ANALOG << GPIO_MODER_MODE4_Pos)  | //Set PA2 to analog (A2)
			(GPIO_ANALOG << GPIO_MODER_MODE11_Pos) | //Set PA4 to analog (A4)
			(GPIO_ANALOG << GPIO_MODER_MODE12_Pos)   //Set PA5 to analog (A5)
			);
	// Configure PB: 0, 1  as analog
	GPIOB->MODER &= ~GPIO_MODER_MODE0_Msk; 					//Set PB0 to analog (D10)
	GPIOB->MODER |= (GPIO_ANALOG << GPIO_MODER_MODE0_Pos);
	GPIOB->MODER &= ~GPIO_MODER_MODE1_Msk; 					//Set PB1 to analog (A3)
	GPIOB->MODER |= (GPIO_ANALOG << GPIO_MODER_MODE1_Pos);

	// Enable ADC clock
	RCC->APBENR2 |= RCC_APBENR2_ADCEN;	// Enable clock access

	// Turn on reference voltage
	ADC1->CR |= ADC_CR_ADVREGEN;

	// wait for the specified startup time (2ms is plenty)]
	delay_ms(2);

	// Now poke the calibrate bit and wait for it to finish
	ADC1->CR |= ADC_CR_ADCAL;
	while (ADC1->CR & ADC_CR_ADCAL);

	// Enable ADC module
		ADC1->ISR |= ADC_ISR_ADRDY;			// Clear the ready bit by writing '1' to it
		ADC1->CR |= ADC_CR_ADEN;			// Set the ADC enable bit
}

/*********************************************/
// Select ADC input channel based on request
void adc_setChannel(unsigned int chNum) {
	switch (chNum) {
	case 0:
		ADC1->CHSELR = ADC_CHSELR_CHSEL0;  // Select channel 0 (PA0/A0)
		break;

	case 1:
		ADC1->CHSELR = ADC_CHSELR_CHSEL1;  // Select channel 1 (PA1/A1)
		break;

	case 2:
		ADC1->CHSELR = ADC_CHSELR_CHSEL4;  // Select channel 4 (PA4/A2)
		break;

	case 3:
		ADC1->CHSELR = ADC_CHSELR_CHSEL18; // Select channel 18 (PB1/A3)
		break;

	case 4:
		ADC1->CHSELR = ADC_CHSELR_CHSEL11; // Select channel 11 (PA11/A4)
		break;

	case 5:
		ADC1->CHSELR = ADC_CHSELR_CHSEL12; // Select channel 12 (PA12/A5)
		break;

	case 6:
		ADC1->CHSELR = ADC_CHSELR_CHSEL17;
		break;

	default:
		ADC1->CHSELR = ADC_CHSELR_CHSEL0;  // Default to channel 0
		break;
	}

}

/*********************************************/
// Set resolution to 12,10, 8, or 6 bits
void adc_setWidth(int width) {
	//clear resolution bits for selection
	ADC1->CFGR1 &= ~ADC_CFGR1_RES_Msk;

	// set resolution bit as requested
	switch (width){
		case 12:
			ADC1-> CFGR1|= (0U << ADC_CFGR1_RES_Pos); // 12 bit resolution
			break;

		case 10:
			ADC1-> CFGR1|= (1U << ADC_CFGR1_RES_Pos); // 10 bit resolution
			break;

		case 8:
			ADC1-> CFGR1|= (2U << ADC_CFGR1_RES_Pos); // 8 bit resolution
			break;

		case 6:
			ADC1-> CFGR1|= (3U << ADC_CFGR1_RES_Pos); // 6 bit resolution
			break;

		default:
			ADC1-> CFGR1|= (0U << ADC_CFGR1_RES_Pos); // default to 12 bit
			break;

	}


}

/*********************************************/

int adc_getValue(void) {
	// Wait until ADC is ready.
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0){}

	ADC1->CR |= ADC_CR_ADSTART;

	// Wait for conversion to finish.
	while( (ADC1->ISR & ADC_ISR_EOC) == 0 ){}		// Blocking

	// Read converted value
	return (int) ADC1->DR;
}

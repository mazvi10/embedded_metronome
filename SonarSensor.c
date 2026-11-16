/* Sonar Sensor 
 * Names  	: Mazvita Nhidza and Amelia Smith
 * Program name	: SonarSensor.c
 * Date created	: 11.14.26
 * Description	: initializes and reads sonar data from a HC-sr04 sonar sensor
 *
 * Dependencies	:
 *
 * I/O pins	:
 *
 * Revisions	:
 *
 */

/* INCLUDE FILES */
#include "SonarSensor.h"

/* CONSTANTS */
#define TRIGGER				(1U << 4)
#define ECHO				(1U << 5)
#define TIM17_PSC_FACTOR	12 // 12MH/12 = 1MHz = 1us
volatile int usTicks = 0;
volatile int timerInterval = 10; 


void sonar_init(void){
	// Enable clock access
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
	
	// Configure trigger pin (PB4) as output
	GPIOB->MODER &= ~GPIO_MODER_MODE4_Msk;
	GPIOB->MODER |= (GPIO_OUTPUT << GPIO_MODER_MODE4_Pos);
	
	// Set trigger pin to low
	GPIOB->ODR &= ~TRIGGER;
	
	// Configure echo pin (PB5) as input
	GPIOB->MODER &= ~GPIO_MODER_MODE5_Msk;
	GPIOB->MODER |= (GPIO_INPUT << GPIO_MODER_MODE5_Pos);
	
	// initlialize timer
	sonarTimer_init(timerInterval);
	
}

int sonar_read(void){
	//set count to 0; 	
	usTicks = 0;
	timerInterval = 0; 

	// set trigger high
	GPIOB->ODR |= TRIGGER;

	// wait 10 seconds
	while (usTicks < 10); 

	// set trigger low
	GPIOB->ODR &= ~TRIGGER;

	// reset usTicks
	usTicks = 0;
	
	// wait for echo to go high 
	while ((GPIOB->IDR & ECHO) == 0) {
		if (usTicks > 38000) {	//timeout after 38 milliseconds
			return 0;
		}
	}

	// reset usTicks
	usTicks = 0;
	
	// count pulse duration
	while ((GPIOB->IDR & ECHO) != 0) {
		if (usTicks > 38000){	//timeout after 38 milliseconds
			return 0;
		}
	}
	// disable timer?
	TIM17->CR1 &= ~TIM_CR1_CEN;

	//calculate distance
	int distance = usTicks / 58;
	return distance;
}
	
void sonarTimer_init(int microseconds) {
	// enable clock access to timer 17
	RCC -> APBENR2 |= RCC_APBENR2_TIM17EN;

	// set prescaler value
	TIM17->PSC = TIM17_PSC_FACTOR - 1;

	// set auto-reload value
	TIM17 -> ARR = microseconds - 1;

	// clear counter
	TIM17->CNT = 0;
	// enable update interrupt
	TIM17->DIER |= TIM_DIER_UIE;

	// enable timerinterupt
	NVIC_EnableIRQ(TIM17_IRQn);

	// enable timer
	TIM17->CR1 |= TIM_CR1_CEN;
	}
void TIM17_IRQHandler(void) {
	__disable_irq();
	/* Clear UIF in status register */
	TIM17->SR &= ~TIM_SR_UIF;
	usTicks += timerInterval; ;
	__enable_irq();
}

	



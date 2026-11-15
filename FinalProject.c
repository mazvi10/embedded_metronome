/* ENGS 28 Final Project
 * Names  		: Mazvita Nhidza and Amelia Smith
 * Program name	: FinalProject.c
 * Date created	: 11.14.26
 * Description	:
 *
 * Dependencies:
 *
 * I/O pins:
 *
 * Revisions:
 *
 */

 /* INCLUDE FILES */
#include <stdio.h>
#include "ES28.h"
#include "uart.h"
#include "i2c.h"
#include "SevenSegPartial.h"

/* FUNCTIONS */
update_servo(void);

/* CONSTANTS */
#define MIN_DISTANCE			// fill in based on sonar info
#define BPM_MAX			200 	// subject to change just an estimate
#define PWM_PSC_FACTOR	60		// pre-scaler
#define PWM_TIMER_MAX	4000	// 20ms
#define SERVO_MIN		200		// 1ms
#define SERVO_MID		300		// 1.5ms
#define SERVO_MAX		400		// 2ms
#define ADC_MAX			4095	// 12-bit converter
#define LED1			(1U<<5)
#define LED2			(1U<<6)
volatile int timerFlag = 0;

/* CODE */
int main (void){
	// Enable Clock?
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// Configure PA5 (RED LED) as an output
	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
	GPIOA->MODER |= (GPIO_OUTPUT << GPIO_MODER_MODE5_Pos);

	// Configure PA6 (GREEN LED) as an output
	GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOA->MODER |= (GPIO_OUTPUT << GPIO_MODER_MODE6_Pos);

	// intialize
	uart1_init(); // Mazvita is working on...
	i2c_init(); // already written
	SevenSeg_init(); // already written
	tim14_pa7_pwm_init(PWM_PSC_FACTOR, PWM_TIMER_MAX);
	adc_init(); 
	timer_init();
	sonar_init(); 

	// define variables
	int pot1Val = 0;
	int pot2Val = 0;
	int bpm = 0;
	int beatInterval = 0;
	int sonarDistance = 0;
	int displayMode = 0;
	int servoEn = 0;
	int lastBeatTime = 0;
	int servoPosition = 0;
	int ledPosition = 0;

	while (1){
		if (timerFlag == 1){
			timerFlag = 0;

			// read first pot value
			setChannel(0);
			pot1Val = adc_read();
			
			// read second pot value
			setChannel (0); 
			pot2Val = adc_read();
			
			// calculate bpm
			bpm = (pot2Val * BPM_MAX) / ADC_MAX;
			if (bpm > 0){
				beatInterval = 60000/bpm;
				flashEn = 1;
			}
			else {
				flashEn = 0;
			}

			// read sonar distance
			sonarDistance = sonar_read(); 

			// enable/disable servo
			if (sonarDistance < MIN_DISTANCE){
				servoEn = 1;
			}
			else {
				servoEn = 0;
			}

			// somehow read bluetooth data

			// determine seven seg mode
			if (pot1Val < 1365){	//bpm display mode
				sevenSegMode = 0; // bpm display mode
			}
			else if (pot1Val <= 2371){
				sevenSegMode = 1; // beat on beat mode
			}
			else {
				sevenSegMode = 2; // speed mode
			}
		}
		if (milliseconds() - lastBeatTime >= beatInterval){

			lastBeatTime = milliseconds();

			// toggle the LEDS
			if (flashEn == 1){
				//toggle leds
				if (ledPositon == 0){
					// Turn on LED 1
					GPIOA->ODR |= LED1;

					// turn off LED 2
					GPIOA->ODR &= ~LED2;

					//update LED Position
					ledPosition = 1;
				}
				else {
					// Turn on LED 2
					GPIOA->ODR |= LED2;

					// Turn of LED 1
					GPIOA->ODR &~ LED1;

					//update LED Position
					ledPosition = 0;
				}
			}
			else {
				GPIOA->ODR &~ LED1;
				GPIOA->ODR &~ LED2;
			}

			// toggle the servo
			if (servoEn == 1){
				if (position == 0){
					update_servo(SERVO_MAX);
					position = 1;
				}
				else{
					update_servo(SERVO_MID)
					position = 0;
				}
			}
		}
	}
}

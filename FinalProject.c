/* ENGS 28 Final Project
 * Names  		: Mazvita Nhidza and Amelia Smith
 * Program name	: FinalProject.c
 * Date created	: 11.14.26
 * Description	:
 *
 * Dependencies:
 *
 * I/O pins:	PA:	0, 1 (Potentiometer 1 and 2)	(INPUT)
 *					5, 6 (LED 1 and 2)				(OUTPUT)
 *					9  (USART 1 Tx )				(ALTERNATE FUNCTION)
 *					10 (USART 1 Rx )				(ALTERNATE FUNCTION)
 *					11 (USART 1 CTS)				(ALTERNATE FUNCTION)
 *					12 (USART 1 RTS)				(ALTERNATE FUNCTION)
 * 				PB: 4 (Sonar Sensor Trigger)		(OUTPUT)
 * 					5 (Sonar Sensor Echo)			(INPUT)
 * 					8, 9 (Seven-Seg Display)		(OUTPUT)
 *
 * Revisions:
 *
 */

 /* INCLUDE FILES */
#include <stdio.h>
#include "ES28.h"
#include "uart.h"
#include "i2c.h"
#include "SevenSeg.h"
#include "ADC.h"
#include "tim.h"
#include "SonarSensor.h"


/* FUNCTIONS */
void update_servo(void);
int uart1_RxAvail(void);
int uart2_RxAvail(void);
void TIM16_IRQHandler(void);

/* CONSTANTS */
#define MIN_DISTANCE	5		// fill in based on sonar info
#define BPM_MAX			200 	// subject to change just an estimate
#define PWM_PSC_FACTOR	60		// pre-scaler
#define PWM_TIMER_MAX	4000	// 20ms
#define SERVO_MIN		200		// 1ms
#define SERVO_MID		300		// 1.5ms
#define SERVO_MAX		400		// 2ms
#define ADC_MAX			4095	// 12-bit converter
#define BUFFER 			25		// potentiometer buffer
#define BT_TIMEOUT 		8000	// 8 second bluetooth timeout 
#define LED1			(1U<<5)
#define LED2			(1U<<6)
volatile int timerFlag = 0;

/* CODE */
int main (void){
	// Enable Clock
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// Configure PA5 (RED LED) as an output
	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk;
	GPIOA->MODER |= (GPIO_OUTPUT << GPIO_MODER_MODE5_Pos);

	// Configure PA6 (GREEN LED) as an output
	GPIOA->MODER &= ~GPIO_MODER_MODE6_Msk;
	GPIOA->MODER |= (GPIO_OUTPUT << GPIO_MODER_MODE6_Pos);

	// intialize
	uart1_init(); 
	uart2_init();
	i2c_init(); 
	SevenSeg_init(); 
	tim14_pa7_pwm_init(PWM_PSC_FACTOR, PWM_TIMER_MAX);
	tim16_ms_interrupt_init(50);
	adc_init(); 
	sonar_init();
	bluefruit_mode_init();
	bluefruit_data_mode();
	
	//test uart1-2 pass through
	printf("UART1-2 passthorugh test \r\n");
	
	// define variables
	int pot1Val = 0;
	int pot2Val = 0;
	uint16_t bpm = 0;
	int beatInterval = 0;
	int sonarDistance = 0;
	int displayMode = 0;
	int servoEn = 0;
	int lastBeatTime = 0;
	int servoPosition = 0;
	int ledPosition = 0;
	int sevenSegMode = 0; 
	int beatDisplay = 0;
	int recievedNum = 0;
	int bluetoothConnected = 0; 
	int lastBluetoothTime = 0; 
	uint8_t display_buffer[2 * HT16K33_NBUF];

	while (1){
		if uart1_RxAvail()){
			int c = uart1_read();
			uart2_write(c);

			// turn incoming characters into an int
			if (c >= '0' && c <= '9'){
				recievedNum = (recievedNum * 10) + (c - '0');
			}
			else if ( c== '\n' || c == '\r'){
				bpm = recievedNum; 
				recievedNum = 0; 
			}
			bluetoothConnected = 1; 
			lastBluetoothTime = milliseconds(); 
		}
		
		if (uart2_(RxAvail()){
			int c = uart2_read();
			uart1_write(c);
		}

		// check if bluetooth has timed out
		if (milliseconds() - lastBluetoothTime > BT_TIMEOUT){
			bluetoothConnected = 0; 
		}
	
		if (timerFlag == 1){
			timerFlag = 0;

			// read first pot value
			setChannel(0);
			pot1Val = adc_read();
			
			// read second pot value
			setChannel (1); 
			pot2Val = adc_read();
			
			// calculate bpm
			if (bluetoothConnected == 0){
				bpm = (pot2Val * BPM_MAX) / ADC_MAX;	
			} 

			// read sonar distance
			sonarDistance = sonar_read(); 

			// enable/disable servo
			if (sonarDistance < MIN_DISTANCE){
				if (bpm < 200 & bpm > 0){
					servoEn = 1;
				}
			}
			else {
				servoEn = 0;
			}

			// determine seven seg mode
			if (pot1Val < (1365-BUFFER)){	//bpm display mode
				sevenSegMode = 0; // bpm display mode
			}
			else if (pot1Val <= (2371-BUFFER) & pot1Val > 1365){
				sevenSegMode = 1; // beat on beat mode
			}
			else if (pot1Val > 2371) {
				sevenSegMode = 2; // speed mode
			}
			else {
				sevenSegMode = 0; 
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

			//seven seg bpm mode
			if (sevenSegMode = 1){
				SevenSeg_number(bpm, display_buffer);
				SevenSeg_write(display_buffer);
			}

			// seven seg speed mode
			if (sevenSegMode = 2){
				if (bpm > 100){
					// seven seg display slow
					display_buffer[0] = 0x71;	// F
					display_buffer[2] = 0x77;	// A
					display_buffer[6] = 0x5B;	// s
					display_buffer[8] = 0x78;	// t
				}
				else if (bpm <= 100){
					display_buffer[0] = 0x00;	// off
					display_buffer[2] = 0x5B;	// s
					display_buffer[6] = 0x38;	// L
					display_buffer[8] = 0x3F;	// 0	
				}
			}

			// seven seg beat mode
			if (sevenSegMode = 1){
				if (beatDisplay = 1){
					// set buffer to say bEAt
					display_buffer[0] = 0x7C;	// b
					display_buffer[2] = 0x79;	// E
					display_buffer[6] = 0x77;	// A
					display_buffer[8] = 0x78;	// t

					// write to seven seg
					SevenSeg_write (display_buffer);

					//reset beat display
					beatDisplay = 0;
				}
				else {
					display_buffer[0] = 0x00;	// off
					display_buffer[2] = 0x00;	// off
					display_buffer[6] = 0x00;	// off
					display_buffer[8] = 0x00;	// off

					// write to seven seg
					SevenSeg_write (display_buffer);

					// update beat display
					beatDisplay = 1;
				}
			}
		}
	}
}

// update servo function
void updateServo(uint16_t value) {
	if (value>SERVO_MAX)
		value = SERVO_MAX;
	else if (value < SERVO_MIN)
		value = SERVO_MIN;
	tim14_pwm_set(value);
}

// timer 16 interupt handler
void TIM16_IRQHandler(void) {
	__disable_irq();
	/* Clear UIF in status register */
	TIM16->SR &= ~TIM_SR_UIF;
	timerFlag = 1;
	__enable_irq();
}

int uart2_rxAvail(void){
	if(USART->ISR & USART_ISR_RXNE_RXFNE) {
		return 1; 
	}
	else{
		return 0;
	}
}

int uart1RxAvail(void){
	if(USART1->ISR & USART_ISR_RXNE_RXFNE){
		return 1;
	}
	else {
		return 0; 
	}
}

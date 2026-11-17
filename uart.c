/*
 * uart.c
 *
 * Modified by Mazvita Nhidza
 *
 */

#include "uart.h"
#include "ES28.h"

#define SYS_FREQ		  12000000U		// standard clock HSISYS (by default scaled down by a factor 4 from HSI48 RC)
#define APB_CLK		  	  SYS_FREQ		// No additional prescaling
#define UART_BAUD_RATE    9600

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate);


/* Remap the system putchar() to use our uart2_write() function. */
int __io_putchar(int ch) {
	uart2_write(ch);
	return ch;
}

/* Initializations */
void uart2_init(void) {
	/*********** configure uart gpio pin ******/
	/* enable clock access to gpioa */
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	/* set PA2 (rx) mode to alternate function mode */
	GPIOA->MODER &= ~GPIO_MODER_MODE2_Msk;
	GPIOA->MODER |= (GPIO_ALTERNATE << GPIO_MODER_MODE2_Pos);

	/* set PA3 (tx) mode to alternate function mode */
	GPIOA->MODER &= ~GPIO_MODER_MODE3_Msk;
	GPIOA->MODER |= (GPIO_ALTERNATE << GPIO_MODER_MODE3_Pos);

	/* set PA2 alternate function type to AF01 which is mapped to UART_TX for PA2 */
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL2_Msk;
	GPIOA->AFR[0] |= (GPIO_AF1 << GPIO_AFRL_AFSEL2_Pos);

	/* set PA3 alternate function type to AF01 which is mapped to UART_RX for PA3 */
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL3_Msk;
	GPIOA->AFR[0] |= (GPIO_AF1 << GPIO_AFRL_AFSEL3_Pos);

	/***********configure uart module **********/
	/* enable clock access to uart2 */
	RCC->APBENR1 |= RCC_APBENR1_USART2EN;

	/* configure baudrate */
	uart_set_baudrate(USART2, APB_CLK, UART_BAUD_RATE);

	/* enable transmitter and receiver, clear other bits (set them back to defaults) */
	USART2->CR1 = USART_CR1_RE | USART_CR1_TE;

	/* enable uart module */
	USART2->CR1 |= USART_CR1_UE;
}

/* UART2 write function (putchar) */
void uart2_write(int ch) {
	/* Make sure the transmit data register is empty */
	while(!(USART2->ISR & USART_ISR_TXE_TXFNF)){} // Block until transmitter ready

	/* Write to transmit data register */
	USART2->TDR = (ch & 0xFF); // restrict to 8 bit
}

/* UART2 read function */
int uart2_read() {
	/* Make sure the receive data register is non-empty */
	while(!(USART2->ISR & USART_ISR_RXNE_RXFNE)){}

	/* Return data register */
	return(USART2->RDR & 0xFF);
}




/* Initializations */
void uart1_init(void) {
	/*********** configure uart gpio pin ******/
	/* enable clock access to gpioa and usart1pu */
	RCC->IOPENR  |= RCC_IOPENR_GPIOAEN;
	RCC->APBENR2 |= RCC_APBENR2_USART1EN;


	/* set PA10 (rx) mode to alternate function mode */
	GPIOA->MODER &= ~GPIO_MODER_MODE10_Msk;
	GPIOA->MODER |= (GPIO_ALTERNATE << GPIO_MODER_MODE10_Pos);

	/* set PA9 (tx) mode to alternate function mode */
	GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk;
	GPIOA->MODER |= (GPIO_ALTERNATE << GPIO_MODER_MODE9_Pos);

	/* set PA11 (cts) mode to alternate function mode */
	GPIOA->MODER &= ~GPIO_MODER_MODE11_Msk;
	GPIOA->MODER |= (GPIO_ALTERNATE << GPIO_MODER_MODE11_Pos);

	/* set PA12 (rts) mode to alternate function mode */
	GPIOA->MODER &= ~GPIO_MODER_MODE12_Msk;
	GPIOA->MODER |= (GPIO_ALTERNATE << GPIO_MODER_MODE12_Pos);

	/* set PA12 alternate function type to AF01 which is mapped to UART_TX for PA12 */
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL12_Msk;
	GPIOA->AFR[1] |= (GPIO_AF1 << GPIO_AFRH_AFSEL12_Pos);

	/* set PA11 alternate function type to AF01 which is mapped to UART_RX for PA11 */
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL11_Msk;
	GPIOA->AFR[1] |= (GPIO_AF1 << GPIO_AFRH_AFSEL11_Pos);

	/* set PA10 alternate function type to AF01 which is mapped to UART_TX for PA10 */
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL10_Msk;
	GPIOA->AFR[1] |= (GPIO_AF1 << GPIO_AFRH_AFSEL10_Pos);

	/* set PA9 alternate function type to AF01 which is mapped to UART_RX for PA9 */
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9_Msk;
	GPIOA->AFR[1] |= (GPIO_AF1 << GPIO_AFRH_AFSEL9_Pos);



	/* configure baudrate */
	uart_set_baudrate(USART1, APB_CLK, UART_BAUD_RATE);

	/* enable transmitter and receiver, clear other bits (set them back to defaults) */
	USART1->CR3 |= USART_CR3_RTSE | USART_CR3_CTSE;
	USART1->CR1 = USART_CR1_RE | USART_CR1_TE;
	USART1->CR1 |= USART_CR1_UE;

	/* enable uart module */
	USART1->CR1 |= USART_CR1_UE;
}

/* UART2 write function (putchar) */
void uart1_write(int ch) {
	/* Make sure the transmit data register is empty */
	while(!(USART1->ISR & USART_ISR_TXE_TXFNF)){} // Block until transmitter ready

	/* Write to transmit data register */
	USART1->TDR = (ch & 0xFF); // restrict to 8 bit
}

/* UART1 read function */
int uart1_read() {
	/* Make sure the receive data register is non-empty */
	while(!(USART1->ISR & USART_ISR_RXNE_RXFNE)){}

	/* Return data register */
	return(USART1->RDR & 0xFF);
}

void bluefruit_mode_init(void)
{
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN;
    GPIOB->BSRR = 1U << 5;  // CMD mode
    GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODE5_Msk) | (1U << GPIO_MODER_MODE5_Pos);  // PB5
}

void bluefruit_data_mode(void)
{
    GPIOB->BRR = 1U << 5;  //  UART/Data mode (reset)
}


/* Set the baud rate for the USART */
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk, uint32_t BaudRate) {
	USARTx->BRR = compute_uart_bd(PeriphClk, BaudRate);
}

/* Determine what value to load into the baud rate register */
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate) {
	/* This formula computes round(PeriphClk / BaudRate) (add 1/2 and truncate) */
	return ((PeriphClk + (BaudRate/2U))/BaudRate);
}



/*
 * uart.h
 *
 *
 */

#ifndef UART_H_
#define UART_H_

int uart2_read();
void uart2_init(void);
void uart2_write(int ch);
int uart1_read();
void uart1_init(void);
void uart1_write(int ch);
int __io_putchar(int ch);
void bluefruit_mode_init(void);
void bluefruit_data_mode(void);


#endif /* UART_H_ */

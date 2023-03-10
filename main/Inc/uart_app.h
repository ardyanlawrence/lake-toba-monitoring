/*
 *  uart_app.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/


#ifndef __UART_APP_H
#define __UART_APP_H

#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "board.h"

#define BUFF_SIZE 1024

uint8_t uart0_evt_buff[BUFF_SIZE];
uint8_t uart1_evt_buff[BUFF_SIZE];
uint8_t uart2_evt_buff[BUFF_SIZE];

void uart0_init(int baud, uart_word_length_t databit, uart_hw_flowcontrol_t flowctrl, uart_parity_t parity,
		uart_stop_bits_t stopbits, uart_sclk_t clk_source);
void uart1_init(int baud, uart_word_length_t databit, uart_hw_flowcontrol_t flowctrl, uart_parity_t parity,
		uart_stop_bits_t stopbits, uart_sclk_t clk_source);
void uart2_init(int baud, uart_word_length_t databit, uart_hw_flowcontrol_t flowctrl, uart_parity_t parity,
		uart_stop_bits_t stopbits, uart_sclk_t clk_source);
		
void uart0_write(const char* data, int len);
void uart1_write(const char* data, int len);
void uart2_write(const char* data, int len);



#endif

/*
 *  modbus_app.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/


#ifndef __MODBUS_APP_H
#define __MODBUS_APP_H

#include "uart_app.h"
#include "board.h"
#include "platform.h"
#include "ntp_app.h"

typedef struct modbus_setting{
	uint8_t modbus_addr;
	uint8_t modbus_function;
	uint8_t modbus_start_add[2];
	uint8_t modbus_len_data[2];
	uint8_t modbus_checksum[2];
	int modbus_rx_len;
} modbus_setting_t;

uint8_t modbus_rx_buffer[512];
int byte_rx_len;

void modbus_request(const char* payload, size_t len);
void modbus_uart_init(int baudrate);
void modbus_app_init(char* settings);
void modbus_application_task(void *arg);


#endif

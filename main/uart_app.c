/*
 *  uart_app.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

#include "uart_app.h"

/*
 *  uart fixed pin: uart0: 43(tx), 44(rx)
 *  				uart1: 17(tx), 18(rx)
 */

uart_config_t uart0_config;
uart_config_t uart1_config;
uart_config_t uart2_config;

QueueHandle_t uart0_queue;
QueueHandle_t uart1_queue;
QueueHandle_t uart2_queue;

// event handling task
void uart0_event_task(void *pvParameters)
{
	uart_event_t u0_event;
	//uart0_evt_buff = malloc(BUFF_SIZE);
	while(1)
	{
		//Waiting for UART event
		if(xQueueReceive(uart0_queue, (void *)&u0_event, (portTickType)portMAX_DELAY))
		{
			bzero(uart0_evt_buff, BUFF_SIZE);
			switch(u0_event.type)
			{
				case UART_DATA:
					uart_read_bytes(UART_NUM_0, uart0_evt_buff, u0_event.size, portMAX_DELAY);
					uart_write_bytes(UART_NUM_0, (const char*) uart0_evt_buff, u0_event.size);
					break;

				default:
					break;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

void uart1_event_task(void *pvParameters)
{
	uart_event_t u1_event;
	//uart1_evt_buff = malloc(BUFF_SIZE);
	while(1)
	{
		//Waiting for UART event
		if(xQueueReceive(uart1_queue, (void *)&u1_event, (portTickType)portMAX_DELAY))
		{
			bzero(uart1_evt_buff, BUFF_SIZE);
			switch(u1_event.type)
			{
				case UART_DATA:
					uart_read_bytes(UART_NUM_1, uart1_evt_buff, u1_event.size, portMAX_DELAY);
					//uart_write_bytes(UART_NUM_1, (const char*) uart1_evt_buff, u1_event.size);
					break;

				default:
					break;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

void uart2_event_task(void *pvParameters)
{
	uart_event_t u2_event;
	//uart1_evt_buff = malloc(BUFF_SIZE);
	while(1)
	{
		//Waiting for UART event
		if(xQueueReceive(uart2_queue, (void *)&u2_event, (portTickType)portMAX_DELAY))
		{
			bzero(uart2_evt_buff, BUFF_SIZE);
			switch(u2_event.type)
			{
				case UART_DATA:
					uart_read_bytes(UART_NUM_2, uart2_evt_buff, u2_event.size, portMAX_DELAY);
					//uart_write_bytes(UART_NUM_1, (const char*) uart1_evt_buff, u1_event.size);
					break;

				default:
					break;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

void uart0_init(int baud, uart_word_length_t databit, uart_hw_flowcontrol_t flowctrl, uart_parity_t parity,
		uart_stop_bits_t stopbits, uart_sclk_t clk_source)
{
	// change uart properties
	uart0_config.baud_rate = baud;
	uart0_config.data_bits = databit;
	uart0_config.flow_ctrl = flowctrl;
	uart0_config.parity = parity;
	uart0_config.stop_bits = stopbits;
	uart0_config.source_clk = clk_source;

	//
	uart_driver_install(UART_NUM_0,BUFF_SIZE,BUFF_SIZE,20,&uart0_queue,0);
	uart_param_config(UART_NUM_0,&uart0_config);
	uart_set_pin(UART_NUM_0, UART0_TX, UART0_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

	xTaskCreate(uart0_event_task, "uart0_event_task", 2048, NULL, UART_RX_TASK_PRIORITY, NULL);
}

void uart1_init(int baud, uart_word_length_t databit, uart_hw_flowcontrol_t flowctrl, uart_parity_t parity,
		uart_stop_bits_t stopbits, uart_sclk_t clk_source)
{
	// change uart properties
	uart1_config.baud_rate = baud;
	uart1_config.data_bits = databit;
	uart1_config.flow_ctrl = flowctrl;
	uart1_config.parity = parity;
	uart1_config.stop_bits = stopbits;
	uart1_config.source_clk = clk_source;

	//
	uart_driver_install(UART_NUM_1,BUFF_SIZE,BUFF_SIZE,20,&uart1_queue,0);
	uart_param_config(UART_NUM_1,&uart1_config);
	uart_set_pin(UART_NUM_1, UART1_TX, UART1_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

	xTaskCreate(uart1_event_task, "uart1_event_task", 2048, NULL, UART_RX_TASK_PRIORITY, NULL);
}

void uart2_init(int baud, uart_word_length_t databit, uart_hw_flowcontrol_t flowctrl, uart_parity_t parity,
		uart_stop_bits_t stopbits, uart_sclk_t clk_source)
{
	// change uart properties
	uart2_config.baud_rate = baud;
	uart2_config.data_bits = databit;
	uart2_config.flow_ctrl = flowctrl;
	uart2_config.parity = parity;
	uart2_config.stop_bits = stopbits;
	uart2_config.source_clk = clk_source;

	//
	uart_driver_install(UART_NUM_2,BUFF_SIZE,BUFF_SIZE,20,&uart2_queue,0);
	uart_param_config(UART_NUM_2,&uart2_config);
	uart_set_pin(UART_NUM_2, UART2_TX, UART2_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

	xTaskCreate(uart2_event_task, "uart2_event_task", 2048, NULL, UART_RX_TASK_PRIORITY, NULL);
}

void uart0_write(const char* data, int len)
{
	uart_write_bytes(UART_NUM_0,data,len);
}

void uart1_write(const char* data, int len)
{
	uart_write_bytes(UART_NUM_1,data,len);
}

void uart2_write(const char* data, int len)
{
	uart_write_bytes(UART_NUM_2,data,len);
}





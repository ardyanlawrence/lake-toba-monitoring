/*
 *  modbus_app.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
 */

/*
 * modbus app using Uart 1 on esp32S2, Pin 17 (TX) and Pin 18 (RX) (ESP32S -> 17 & 16)
 *
 * modbus RTU protocol example
 *
 * Transmit example:
 * 01 03 0300 0004 444D
 * 01 -> modbus addr
 * 03 -> modbus function code
 * 0300 -> start address in hex
 * 0004 -> length data to be read
 * 444D -> check sum (crc 16 word swap)
 *
 * receive:
 * 01 03 08 014D 0138 0138 0016 68F4
 * 01 -> modbus addr
 * 03 -> function code
 * 08 -> length of byte data
 * 014D -> 2 bytes of first data at start address 0300
 * 0138 -> 2 bytes of second data at next address 0301
 * 0138 -> 2 bytes of third data at next address 0302
 * 0016 -> 2 bytes of fourth data at next address 0303
 * 68F4 -> 2 bytes of checksum
 *
 * calculating the data length response
 * requested data len = 4, response data should be have 4x2 = 8 bytes
 * added add,function code, length data, data, check sum = 1+1+1+8+2 = 13 bytes data in buffer,
 * so if requested len = 4, there should be 13 bytes in buffer, so the formula = 4x2+5
 */

#include "modbus_app.h"

SemaphoreHandle_t modbus_req_sem = NULL;

void modbus_app_init(char *settings)
{
    printf("running modbus sensor\r\n");
	static uint8_t task_i = 0;
	char th_name[32];
	sprintf(th_name, "MODBUS_TASK%i", task_i);
    task_i++;
	modbus_uart_init(9600);
	xTaskCreate(modbus_application_task, th_name, DEFAULT_SENSOR_STACK_SIZE, settings, tskIDLE_PRIORITY, NULL);
}

void modbus_application_task(void *arg)
{
	char request[32];
	char sensor_name[32];
	char param_name[32];
	int sampling;
	char *msg = (char *)arg;

	sscanf(strstr(msg, "raw"), "raw\":\"%[^\"]\"", request);
    sscanf(strstr(msg, "sampling"), "sampling\":%d", &sampling);
    sscanf(strstr(msg, "sensor"), "sensor\":\"%[^\"]\"", sensor_name);
    sscanf(strstr(msg, "param"), "param\":\"%[^\"]\"", param_name);
	printf("request:%s, sampling:%d, sensor:%s, param:%s\r\n", request, sampling, sensor_name, param_name);

	TickType_t lastWakeTime = xTaskGetTickCount();
	while (1)
	{
		int counter = 0;
		char payload_tmp[3];
		char modbus_payload[256];
		bzero(modbus_payload, sizeof(modbus_payload));
		modbus_request(request, strlen(request));
		for (int i = 0; i < byte_rx_len; i++)
		{
			sprintf(payload_tmp, "%.2X", modbus_rx_buffer[i]);
			for (int j = 0; j < 2; j++)
			{
				modbus_payload[counter] = payload_tmp[j];
				counter++;
			}
		}

		// pack data
		int64_t timestamp = ntp_to_epoch() / 1000;
		char buffer[512];
		sprintf(buffer, "{\"ts\":%llu,\"channel\":\"MODBUS\",\"sensor_key\":\"%s:data:%s\",\"raw\":\"%s\"}", 
			timestamp, sensor_name, param_name, modbus_payload);
		printf("buffer payload: %s\r\n", buffer);

		// save to sdcard
		char backup_name[8];
		sprintf(backup_name, "%i", (uint16_t)(timestamp / 1000));
		printf("filename: %s\r\n", backup_name);
		sdcard_write_file(MODBUS_BACKUP_FOLDER, backup_name, buffer);

		// check data in backup folder
		int filenum = sdcard_countfile(MODBUS_BACKUP_FOLDER);
		for (int i = 0; i < filenum; i++)
		{
			// publish
			release_procedure(MODBUS_BACKUP_FOLDER);
			vTaskDelay(pdMS_TO_TICKS(200));
		}
		
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(sampling*1000)); // delay

		bzero(buffer, sizeof(buffer));
	}
}

void modbus_uart_init(int baudrate)
{
	// init uart 2
	uart2_init(baudrate, UART_DATA_8_BITS, UART_HW_FLOWCTRL_DISABLE,
			   UART_PARITY_DISABLE, UART_STOP_BITS_1, UART_SCLK_APB);

	modbus_req_sem = xSemaphoreCreateMutex();
}

void modbus_request(const char *payload, size_t len)
{
	uint16_t len_req = 0x0000;
	uint8_t payload_byte[32];

	bzero(modbus_rx_buffer, sizeof(modbus_rx_buffer));
	bzero(uart2_evt_buff, sizeof(uart2_evt_buff));

	if (xSemaphoreTake(modbus_req_sem, (TickType_t)1000))
	{
		// send modbus request
		for (unsigned int i = 0; i < len; i += 2) 
		{
			sscanf(payload + i, "%2hhx", &payload_byte[i/2]);
		}

		uart2_write((const char*) payload_byte, len/2);
		vTaskDelay(pdMS_TO_TICKS(MODBUS_WAIT_RESPONSE));

		// receive data
		len_req = (len_req|payload_byte[4])<<8;
		len_req = len_req|payload_byte[5];
		byte_rx_len = len_req*2+5;
		for (int i = 0; i < byte_rx_len; i++)
		{
			// get receive buffer uart 2
			modbus_rx_buffer[i] = uart2_evt_buff[i];
		}
		printf("\r\n");
		xSemaphoreGive(modbus_req_sem);
	}
}
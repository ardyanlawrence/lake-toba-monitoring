/*
 *  ble_server.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/


#ifndef __BLE_SERVER_H
#define __BLE_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_wifi.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "sdkconfig.h"

void ble_server_init(void);
void ble_server_load_message(uint8_t* msg, uint16_t len);
void ble_server_read_message(void);
void ble_server_write_message(char* msg, int len);
void ble_server_parse_message(void);
void ble_server_disable(void);

// for test
char total_server_msg[1024];
int server_msg_len;
bool connect_wifi;
bool server_msg_received_flag;
uint8_t prov_ssid[32];
uint8_t prov_pass[64];

#endif


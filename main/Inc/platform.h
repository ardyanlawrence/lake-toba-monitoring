/*
 *  platform.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/3/2022
 * 	revison	:
 *
*/

#ifndef PLATFORM_H
#define PLATFORM_H
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "cJSON.h"
#include "sd_card.h"
#include "board.h"
#include "mqtt_app.h"
#include "mqtt_auth.h"
#include "wifi_app.h"

void platform_mqtt_handler(char *mqtt_topic, char *mqtt_data, uint8_t topic_type);
void remove_char(char* payload, char target_char, int len_payload, char* result);
void remove_char2(char* payload, char target_char, char* result);
void clean_data(char* payload, int len_payload, char* result);
void clean_data_sdcard(char* payload, char* result);
int release_procedure(char* foldername);
void platform_settings_device(bool ps_stat);
bool platform_settings_status(void);
bool sensors_query_status(bool is_online);

#endif

//EOF


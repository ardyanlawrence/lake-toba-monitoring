/*
 *  esp_config.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 11/3/2022
 * 	revison	:
 *
*/

#ifndef ESP_CONFIG_H
#define ESP_CONFIG_H

#include <stdio.h>
#include <string.h>
#include "board.h"
#include "sd_card.h"
#include "mqtt_app.h"
#include "cJSON.h"
#include "platform.h"

void load_configuration(void);
void mqtt_load_setting(void);
void write_configuration(char* settings);
void config_sensornode(char* settings);
void parse_sensor_settings(char* sampling_settings, char* sensors_data);
void load_sensor_settings();

// modbus setting
uint8_t MODBUS_CONFIG[6];


// payload settings
char mqtt_username[21];
char mqtt_password[65];
char mqtt_uid[13];

#endif
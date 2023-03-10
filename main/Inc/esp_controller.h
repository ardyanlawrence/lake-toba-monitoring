/*
 *  esp_controller.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

#ifndef __ESP_CONTROLLER_H
#define __ESP_CONTROLLER_H

#include "esp_config.h"
#include "ntp_app.h"
#include "power_management.h"
#include "sd_card.h"
#include "mqtt_app.h"
#include "platform.h"
#include "wifi_app.h"
#include "board.h"
#include "ble_client.h"
#include "ble_server.h"
#include "provisioning.h"
#include "sensors.h"
#include "rgb.h"

#define PROVISIONING_PRESS_TIME 60000
#define DEBUG_PRESS_TIME 20000
#define REBOOT_PRESS_TIME 5000

#define MAIN_CMD_DEBUG 0
#define MAIN_CMD_PROVISIONING 1
#define MAIN_EV_NOT_CONFIGURED 2
#define MAIN_EV_CONFIGURED 3
#define MAIN_EV_NOT_WIFI 4
#define MAIN_EV_WIFI 5
#define MAIN_EV_WRONG_WIFI 6
#define MAIN_EV_NOT_PLATFORM 7
#define MAIN_EV_PLATFORM 8


// regular mode

void button_led_fsm(void *arg);
void main_fsm(void *arg);
void provisioning_mode();
void platform_settings_mode(bool ps_stat);

void create_empty_sd_dir();

#endif

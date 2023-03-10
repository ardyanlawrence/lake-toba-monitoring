/*
 *  ntp_app.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

#ifndef __NTP_APP_H
#define __NTP_APP_H

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "esp_sntp.h"
#include "board.h"
#include "eeprom_app.h"
#include "wifi_app.h"

typedef enum{
	NTP_SYNC_OK = 0,
	NTP_SYNC_FAIL
}ntpsync_stat_t;

void ntp_init(const char* ntp_server);
int64_t ntp_to_epoch(void);


#endif

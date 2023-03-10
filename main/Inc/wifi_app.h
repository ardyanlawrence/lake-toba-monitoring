/*
 *  wifi_app.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/


#ifndef __WIFI_APP_H
#define __WIFI_APP_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_bit_defs.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/inet.h"
#include "lwip/ip4_addr.h"
#include "lwip/dns.h"

#include "rgb.h"

uint8_t wifi_global_stat;

void wifi_init(uint8_t* ssid, uint8_t* pass, wifi_auth_mode_t auth_mode);
void wifi_init_sta(uint8_t* ssid, uint8_t* pass, wifi_auth_mode_t auth_mode);
esp_err_t wifi_reconnect(void);
uint8_t read_wifi_stat(void);
void dns_found_cb(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
void wifi_resolve_dns();

int8_t RSSI_WIFI;



#endif

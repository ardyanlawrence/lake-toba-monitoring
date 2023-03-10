/*
 *  mqtt_app.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/


#ifndef __MQTT_APP_H
#define __MQTT_APP_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "platform.h"
#include "rgb.h"
//#include "http_parser.h"

extern const char *mqtt_data;
extern const char *mqtt_uid_sub;
extern const char *mqtt_uid_pub;
extern const char *tid_query_result;
extern const char *tsensors_query_result;

// legacy but optimized topic
// - mqtt_data_topic
// - mqtt_data_image_topic
// iotera/pub/1000000091/f2f6c6a1-33bd-4db0-99c0-5d4f5dba94f0/...
char mqtt_data_topic_prefix[MQTT_PREFIX_LEN];

// new topic based on UID - subscribe
// - mqtt_uid_subscribe_wildcard_topic = +
// - mqtt_uid_id_query_subscribe_topic = id_query_result
// - mqtt_uid_sensors_query_subscribe_topic: reply from platform = sensors_query_result
// iotera_uid/sub/0052245130d0/...
char mqtt_uid_sub_topic_prefix[MQTT_UID_PREFIX_LEN];

// new topic based on UID - subscribe
// - mqtt_uid_id_query_topic = id_query
// - mqtt_uid_sensors_query_topic: querying sensors on restart = sensors_query
// - mqtt_uid_config_result_topic: confirm upon receive config update from platform = config_result
// - mqtt_uid_command_result_topic: confirm upon receive command from platform = command_result
// - mqtt_uid_root_config_result_topic = root_config_result
// - mqtt_uid_root_command_result_topic = root_command_result
// - mqtt_uid_logging_result_topic: real-time log = logging_result
// - mqtt_uid_pong_topic: upon receive 'ping' ie. 'ping_result' = pong
// - mqtt_uid_sensors_result_topic: publish hasil dari sensors yang diminta platform, jika kebetulan alat online = sensors_result
// - mqtt_uid_version_result_topic = version_result
// - mqtt_uid_log_query_result_topic: historical log based on query param = log_query_result

char mqtt_uid_pub_topic_prefix[MQTT_UID_PREFIX_LEN];

int mqtt_global_stat;
void mqtt_init(const char* mqtt_server, int mqtt_port, char* username, char* pass, char* uid, bool mqtt_ver);
int mqtt_publish(const char* topic, char* payload);
int mqtt_publish_iotera(char* payload);
int mqtt_subscribe(const char* topic);
int mqtt_conn_stat(void);
void mqtt_data_handling(esp_mqtt_event_handle_t event);
void generate_topic();
void generate_uid_topic(const char* uid);
void load_topic();
void get_id_query();
void get_sensors_query();
void remove_quotes(char* line);

// payload
//char sub_msg[16];
char mqtt_msg_payload[128];

#endif

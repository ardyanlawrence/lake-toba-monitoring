/*
 *  mqtt_app.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/


/*
 * MQTT Global stat
 * 	0 -> mqtt error
 *	1 -> connected
 *	2 -> disconnected
 *	3 -> subscribed
 *	4 -> unsubscribed
 *	5 -> published
 *	6 -> Data
 *
 *
 *  return message_id of the publish message (for QoS 0 message_id will always be zero) on success.
 *         -1 on failure.
 */


#include "mqtt_app.h"

esp_mqtt_client_handle_t client;
static const char *TAG = "MQTT_EXAMPLE";

char application_id[11];
char device_id[40];
char tmp_mqtt_topic[100];

// base prefix untuk topics
// akan dicombine dengan app_id / device_id / uid di generate_(uid_)topic()
const char *mqtt_data = "iotera/pub";
const char *mqtt_uid_sub = "iotera_uid/sub";
const char *mqtt_uid_pub = "iotera_uid/pub";

// suffix for subscribe
const char *tid_query_result = "id_query_result";
const char *tsensors_query_result = "sensors_query_result";

uint8_t num_connected = 0;
int msg_id;
SemaphoreHandle_t mqtt_pub_iotera_sem = NULL;

char mqtt_uid[13];
bool mqtt_version;
bool is_query_finished = false;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_UID_EVENT_CONNECTED");
			turn_on_rgb(RGB_GREEN);
            mqtt_global_stat = 1;
			bzero(tmp_mqtt_topic, sizeof(tmp_mqtt_topic));
			sprintf(tmp_mqtt_topic, "%s/+", mqtt_uid_sub_topic_prefix);
			mqtt_subscribe(tmp_mqtt_topic);
			// if platform settings is false, get application_id and device_id
			if (!mqtt_version) get_id_query();
			else { // is platform settings is true, just reload sensors query
				// only on 1st connection after reboot
				if (num_connected == 0) get_sensors_query();
			}
			num_connected++;
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_UID_EVENT_DISCONNECTED");
            mqtt_global_stat = 2;
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_UID_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            mqtt_global_stat = 3;
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_UID_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            mqtt_global_stat = 4;
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_UID_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            mqtt_global_stat = 5;
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_UID_EVENT_DATA");
            mqtt_data_handling(event);
            mqtt_global_stat = 6;
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_UID_EVENT_ERROR");
            mqtt_global_stat = 0;
            break;
        default:
            ESP_LOGI(TAG, "MQTT_UID Other event id:%d", event->event_id);
            break;
    }

    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_init(const char* mqtt_server, int mqtt_port, char* username, char* pass, char* uid, bool mqtt_ver)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_SENSORNODE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());

	// generate topic for iotera platform
	// 0 = UID version, 1 = APP_ID and DEVICE_ID version
    mqtt_version = mqtt_ver;

	sprintf(mqtt_uid,"%s",uid);

	uint8_t filenum = sdcard_countfile(PROVISIONING_FOLDER);
	printf("prov cnt file: %d\r\n", filenum);
	// if there are no uid, user, pass
	if (!sdcard_check_file_exist(PROVISIONING_FOLDER,"uid")) {
		printf("writing uid provis file\r\n");
		sdcard_write_file(PROVISIONING_FOLDER,"uid",mqtt_uid);
	}
	if (!sdcard_check_file_exist(PROVISIONING_FOLDER,"user")) {
		printf("writing user provis file\r\n");
		sdcard_write_file(PROVISIONING_FOLDER,"user",username);
	}
	if (!sdcard_check_file_exist(PROVISIONING_FOLDER,"pass")) {
		printf("writing pass provis file\r\n");
		sdcard_write_file(PROVISIONING_FOLDER,"pass",pass);
	}
	// if (filenum < 3) {
	// 	sdcard_write_file(PROVISIONING_FOLDER,"uid",mqtt_uid);
	// 	sdcard_write_file(PROVISIONING_FOLDER,"user",username);
	// 	sdcard_write_file(PROVISIONING_FOLDER,"pass",pass);
	// }

	filenum = sdcard_countfile(TOPICS_FOLDER);
	printf("top cnt file: %d, mqtt_version: %d\r\n", filenum, mqtt_version);

	if (filenum>2) load_topic(); // if there are topics, load
	else { // if topics are not there yet, generate
		generate_uid_topic(uid);
		// if mqtt_version is true, also generate data and data_image topic
		if (mqtt_version == true) {
			generate_topic();
		}
	}
	
    esp_mqtt_client_config_t mqtt_cfg = {
    		.uri = mqtt_server,
    		.port = mqtt_port,
    		.username = username,
    		.password = pass,
			.buffer_size = 4095
    };
    
    heap_caps_check_integrity_all(true);
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

	mqtt_pub_iotera_sem = xSemaphoreCreateMutex();
}

/*
 *
 * APPLICATION FOR MQTT
 *
 *
 */
int mqtt_publish(const char* topic, char* payload)
{
	int pub_stat = 0;
	if ((mqtt_global_stat != 0) && (mqtt_global_stat != 2)){
		// publish payload
		// printf("topic: %s, payload: %s\r\n", topic, payload);
		pub_stat = esp_mqtt_client_publish(client,topic,payload,0, 1, 0);
	}

	return pub_stat;
}

int mqtt_publish_iotera(char* payload)
{
	int pub_stat = 0;
	//if( xSemaphoreTake(mqtt_pub_iotera_sem, (TickType_t) 3000)){
		if ((mqtt_global_stat != 0) && (mqtt_global_stat != 2)){
			// publish payload
			bzero(tmp_mqtt_topic, sizeof(tmp_mqtt_topic));
			sprintf(tmp_mqtt_topic, "%s/data", mqtt_data_topic_prefix);
			pub_stat = esp_mqtt_client_publish(client,tmp_mqtt_topic,payload,0, 1, 0);
			vTaskDelay(pdMS_TO_TICKS(100));
		}
		//xSemaphoreGive(mqtt_pub_iotera_sem);
	//}

	return pub_stat;
}


int mqtt_subscribe(const char* topic)
{
	int stat = 0;
	if ((mqtt_global_stat != 0) && (mqtt_global_stat != 2)) {
		// publish payload
		stat = esp_mqtt_client_subscribe(client,topic,0);
	}
	return stat;
}

int mqtt_conn_stat(void)
{
	int stat;
	esp_err_t status = ESP_OK;
	if (mqtt_global_stat == 2){
		status = esp_mqtt_client_reconnect(client);
	}
	if(status == ESP_OK){
		stat = 0;
		//printf("connected to server\r\n");
	}
	else {
		//printf("disconnected from server\r\n");
		stat = -1;}

	return stat;
}

void mqtt_data_handling(esp_mqtt_event_handle_t data_event)
{
	uint8_t topic_type = 0;

	bzero(mqtt_msg_payload,sizeof(mqtt_msg_payload));
    printf("TOPIC=%.*s\r\n", data_event->topic_len, data_event->topic);
    // printf("DATA=%.*s\r\n", data_event->data_len, data_event->data);
	
	// Get the topic
	char *topic = malloc(data_event->topic_len+1);
	memcpy(topic, data_event->topic, data_event->topic_len);
	topic[data_event->topic_len] = '\0';

	// Get the data
	char *data = malloc(data_event->data_len+1);
	memcpy(data, data_event->data, data_event->data_len);
	data[data_event->data_len] = '\0';

	if (strstr(data_event->topic, mqtt_uid_sub) != NULL) { // subscribe topic
		if (strstr(data_event->topic, tid_query_result) != NULL) topic_type = 1;
		else if (strstr(data_event->topic, tsensors_query_result) != NULL) topic_type = 2;

		platform_mqtt_handler(topic, data, topic_type);	
		
		if (topic_type > 0) is_query_finished = true;
	}
}

void generate_topic()
{
	bzero(application_id,sizeof(application_id));
	bzero(device_id,sizeof(device_id));
	sdcard_read_file(PROVISIONING_FOLDER, "appid",application_id);
	sdcard_read_file(PROVISIONING_FOLDER, "devid",device_id);
	sprintf(mqtt_data_topic_prefix,"%s/%s/%s",mqtt_data, application_id,device_id);
	sdcard_write_file(TOPICS_FOLDER,"data",mqtt_data_topic_prefix);
}

void generate_uid_topic(const char* uid)
{
	sprintf(mqtt_uid_sub_topic_prefix,"%s/%s",mqtt_uid_sub,uid);
	sprintf(mqtt_uid_pub_topic_prefix,"%s/%s",mqtt_uid_pub,uid);
	sdcard_write_file(TOPICS_FOLDER,"usub",mqtt_uid_sub_topic_prefix);
	sdcard_write_file(TOPICS_FOLDER,"upub",mqtt_uid_pub_topic_prefix);
}

void load_topic() {
	bzero(mqtt_data_topic_prefix,sizeof(mqtt_data_topic_prefix));
	bzero(mqtt_uid_sub_topic_prefix,sizeof(mqtt_uid_sub_topic_prefix));
	bzero(mqtt_uid_pub_topic_prefix,sizeof(mqtt_uid_pub_topic_prefix));
	sdcard_read_file(TOPICS_FOLDER, "data", mqtt_data_topic_prefix);
	sdcard_read_file(TOPICS_FOLDER, "usub", mqtt_uid_sub_topic_prefix);
	sdcard_read_file(TOPICS_FOLDER, "upub", mqtt_uid_pub_topic_prefix);
}

void get_id_query() {
	char payload[50];
	uint8_t attempt = 0;
	sprintf(payload,"{\"id\":\"%s\"}",mqtt_uid);
	bzero(tmp_mqtt_topic, sizeof(tmp_mqtt_topic));
	sprintf(tmp_mqtt_topic, "%s/id_query", mqtt_uid_pub_topic_prefix);

	while (!is_query_finished && (attempt<1)) {
		mqtt_publish(tmp_mqtt_topic, payload);
		vTaskDelay(pdMS_TO_TICKS(1000));
		attempt++;
	}
}

void get_sensors_query() {
	char payload[50];
	uint8_t attempt = 0;
	sprintf(payload,"{\"id\":\"%s\"}",mqtt_uid);
	bzero(tmp_mqtt_topic, sizeof(tmp_mqtt_topic));
	sprintf(tmp_mqtt_topic, "%s/sensors_query", mqtt_uid_pub_topic_prefix);

	while (!is_query_finished && (attempt<1)) {
		mqtt_publish(tmp_mqtt_topic, payload);
		vTaskDelay(pdMS_TO_TICKS(1000));
		attempt++;
	}
}

void remove_quotes(char* line) {
	int j = 0;
	for (int i = 0; i < strlen(line); i ++) {
		if (line[i] != '"' && line[i] != '\\') { 
			line[j++] = line[i];
		}
	}
	if(j>0) line[j]=0;
}

// EOF



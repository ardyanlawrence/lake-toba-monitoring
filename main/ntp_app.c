/*
 *  ntp_app.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

#include "ntp_app.h"

static const char *TAG = "example";
const char ntp_serv[] = "pool.ntp.org";

ntpsync_stat_t NTP_Sync_stat;
SemaphoreHandle_t ntp_epoch_sem = NULL;

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
    NTP_Sync_stat = NTP_SYNC_OK;
}

void ntp_init(const char* ntp_server)
{
	ESP_ERROR_CHECK( nvs_flash_init() );
	ESP_ERROR_CHECK(esp_netif_init());
	//ESP_ERROR_CHECK( esp_event_loop_create_default() );

	//initialize_sntp();
    ESP_LOGI(TAG, "Initializing SNTP, server: %s",ntp_server);
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, ntp_server);
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
	sntp_set_sync_interval(60000);
    sntp_init();

	// wait for time to be set
	int retry = 0;
	const int retry_count = 100;

	while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
		ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
		vTaskDelay(pdMS_TO_TICKS(2000));
	}	
	//sntp_set_sync_interval(60000);

	ntp_epoch_sem = xSemaphoreCreateMutex();
}

int64_t ntp_to_epoch(void)
{
	int64_t epoch;
	xSemaphoreTake(ntp_epoch_sem, (TickType_t) 500);
	// get data from local RTC
	struct timeval tv_now;
	gettimeofday(&tv_now, NULL);
	epoch = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
	//printf("epoch: %lli\r\n",epoch);
	xSemaphoreGive(ntp_epoch_sem);
	return epoch;
}


//EOF

/*
 *  esp_controller.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
 */

/*
 * Brief:
 * this file is the main controller of routine from all measurement and communication
 * then executed in main file
 * mainly about,
 * 1. measure sensor
 * 2. send data
 * 3. sleep
 */
#include "esp_controller.h"

// Payload
// settings storage
uint8_t wifi_auth;

// NTP
const char ntp_server[] = "pool.ntp.org";
char settings[512];
char provision_settings[256];

QueueHandle_t main_cmdqueue;
QueueHandle_t main_evqueue;

/*
 * Press button to activate Debug / perform Reboot / enter Provisioning-Platform Settings mode
 * Send message via queue for main_fsm
 * RGB LED indicates Debug / run mode and errors / Provisioning-Platform Settings mode
 */
void button_led_fsm(void *arg)
{
	printf("Start button_led_fsm\r\n");
	uint8_t BUTTON_MODEValue = 0;
	uint8_t lastBUTTON_MODEValue = 0;
	int64_t pressedTime = 0; // in millis, so divide esp_timer_get_time() by 1000
	int64_t releasedTime = 0;
	int64_t pressDuration = 0;

	uint32_t ulVar;

	gpio_initialize(BUTTON_MODE, GPIO_MODE_INPUT, GPIO_INTR_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_DISABLE);
	gpio_set_direction(LED_INDICATOR, GPIO_MODE_OUTPUT);
	while (1)
	{
		// Check Button Mode press
		BUTTON_MODEValue = gpio_get_level(BUTTON_MODE);
		if (lastBUTTON_MODEValue != BUTTON_MODEValue)
		{
			if (lastBUTTON_MODEValue == 0 && BUTTON_MODEValue == 1)
				pressedTime = esp_timer_get_time() / 1000;
			else if (lastBUTTON_MODEValue == 1 && BUTTON_MODEValue == 0)
				releasedTime = esp_timer_get_time() / 1000;
			pressDuration = releasedTime - pressedTime;
			printf("releasedTime: %lld, pressedTime: %lld, pressDuration: %lld\r\n", releasedTime, pressedTime, pressDuration);

			if (pressDuration > REBOOT_PRESS_TIME && pressDuration < DEBUG_PRESS_TIME)
			{
				printf("Button Mode: Reboot\r\n");
				esp_restart();
			}
			else if (pressDuration > DEBUG_PRESS_TIME && pressDuration < PROVISIONING_PRESS_TIME)
			{
				printf("Button Mode: activate Debug\r\n");
				ulVar = MAIN_CMD_DEBUG;
				xQueueSend(main_cmdqueue, (void *)&ulVar, NULL);
			}
			else if (pressDuration > PROVISIONING_PRESS_TIME)
			{
				printf("Button Mode: enter Provisioning\r\n");
				ulVar = MAIN_CMD_PROVISIONING;
				xQueueSend(main_cmdqueue, (void *)&ulVar, NULL);
			}
			lastBUTTON_MODEValue = BUTTON_MODEValue;
		}
		vTaskDelay(pdMS_TO_TICKS(100));
		// Update LED blinking
	}
}

/*
 *  as the name suggest, main code of SensorNode
 */
void main_fsm(void *arg)
{
	printf("Start main_fsm\r\n");
	init_rgb();
	turn_on_rgb(RGB_RED);
	// create queue for communication between main fsm task and button led task
	main_evqueue = xQueueCreate(30, sizeof(uint32_t));
	main_cmdqueue = xQueueCreate(30, sizeof(uint32_t));
	uint32_t main_cmd;

	// sdcard init
	sdcard_init(SPI_MOSI, SPI_MISO, SPI_CLK, SPI_CS);

	// check if provisioned or not
	turn_on_rgb(RGB_MAGENTA);
	bzero(provision_settings, sizeof(provision_settings));
	bool prov_stat = false;
	printf("check provisioning status\r\n");
	prov_stat = provisioning_status();
	printf("provisioning status = %i\r\n", prov_stat);

	if (prov_stat == false)
		provisioning_mode();

	// load setting from sdcard
	bzero(provision_settings, sizeof(provision_settings));
	sdcard_read_file(PROVISIONING_FOLDER, "wifi", provision_settings);
	printf("%s\r\n", provision_settings);
	parse_prov_setting(provision_settings, strlen(provision_settings));

	// if ssid is empty / wifi-pass format is wrong, do not bother trying
	if (prov_ssid[0] != 0)
	{
		wifi_init(prov_ssid, prov_pass, WIFI_AUTH_WPA2_PSK);
	}

	// check if connected to AP
	// if online, load sensors query from platform
	bool config_stat = false;
	if (read_wifi_stat() == 1)
	{
		bool ps_stat = false;
		printf("check platform_settings status\r\n");
		ps_stat = platform_settings_status();
		// get app_id - device_id, init mqtt, and reload sensors query
		platform_settings_mode(ps_stat);
		while (config_stat == false)
		{
			config_stat = sensors_query_status(true);
		}
	}
	// else if offline, check sensors query config in SD card
	else
	{
		config_stat = sensors_query_status(false);
	}

	if (config_stat)
	{
		// finished provisioning and platform_settings
		printf("finished init mqtt and reload sensors query. now loading sensor config\r\n");
		load_configuration();
		sensor_run();
	}
	else
	{
		printf("no internet and failed to load sensors query config. rebooting...\r\n");
		esp_restart();
	}

	while (1)
	{
		if (xQueueReceive(main_cmdqueue, &main_cmd, portMAX_DELAY))
		{
			switch (main_cmd)
			{
			case MAIN_CMD_DEBUG:
				printf("main_fsm: activate Debug\r\n");
				break;
			case MAIN_CMD_PROVISIONING:
				printf("main_fsm: enter Provisioning\r\n");
				provisioning_mode();
				break;
			default:
				printf("main_fsm: unknown Main CMD\r\n");
				break;
			}
		}

		// old code
		//  get_sensor_data_regular_mode();
		//  send_sensor_data_regular_mode();
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

/*
 * Enter by pressing button in 60s
 * User must enter wifi correctly
 * Intention is to allow provisioning SensorNode even WiFi is not in vicinity
 * If wifi is input incorrectly, user must press Button Mode manually to enter Provisioning Mode again
 */
void provisioning_mode()
{
	printf("Entering Provisioning mode\r\n");
	create_empty_sd_dir();
	// turn on BLE and wait for wifi credentials
	provisioning_device();
}

/*
 * Enter by restart with prov_stat is true
 * If no internet, wait until internet is up
 * Meanwhile, user can re-input wifi by pressing Button Mode
 */
void platform_settings_mode(bool ps_stat)
{
	printf("Entering Platform Setting mode\r\n");
	sdcard_write_file(PROVISIONING_FOLDER, "sqstat", "0");

	// waiting internet up
	wifi_resolve_dns();
	// init ntp
	ntp_init(ntp_server);
	// getting application_id, device_id, then download sensor config
	platform_settings_device(ps_stat);
}

/*
 * Only call this during Provisioning
 */
void create_empty_sd_dir()
{
	vTaskDelay(pdMS_TO_TICKS(100));
	sdcard_mkdir(BACKUPS_FOLDER);
	vTaskDelay(pdMS_TO_TICKS(100));
	sdcard_mkdir(SETTINGS_FOLDER);
	vTaskDelay(pdMS_TO_TICKS(100));
	sdcard_mkdir(TOPICS_FOLDER);
	vTaskDelay(pdMS_TO_TICKS(100));
	sdcard_mkdir(PROVISIONING_FOLDER);
	vTaskDelay(pdMS_TO_TICKS(100));
	sdcard_mkdir(SENSOR_SETTINGS_FOLDER);
	// remove all file in folder settings
	sdcard_remove_in_folder(SETTINGS_FOLDER);
	sdcard_remove_in_folder(SENSOR_SETTINGS_FOLDER);
	vTaskDelay(pdMS_TO_TICKS(100));
	sdcard_mkdir(ANALOG1_BACKUP_FOLDER);
	sdcard_mkdir(ANALOG2_BACKUP_FOLDER);
	sdcard_mkdir(ANALOG3_BACKUP_FOLDER);
	sdcard_mkdir(ANALOG1_INSTANCE_FOLDER);
	sdcard_mkdir(ANALOG2_INSTANCE_FOLDER);
	sdcard_mkdir(ANALOG3_INSTANCE_FOLDER);
	sdcard_mkdir(DIGITAL1_BACKUP_FOLDER);
	sdcard_mkdir(DIGITAL2_BACKUP_FOLDER);
	sdcard_mkdir(DIGITAL3_BACKUP_FOLDER);
	sdcard_mkdir(COUNTER1_BACKUP_FOLDER);
	sdcard_mkdir(COUNTER2_BACKUP_FOLDER);
	sdcard_mkdir(COUNTER3_BACKUP_FOLDER);
	sdcard_mkdir(FREQ1_BACKUP_FOLDER);
	sdcard_mkdir(FREQ2_BACKUP_FOLDER);
	sdcard_mkdir(FREQ3_BACKUP_FOLDER);
	sdcard_mkdir(MODBUS_BACKUP_FOLDER);
	sdcard_mkdir(UART_BACKUP_FOLDER);
	vTaskDelay(pdMS_TO_TICKS(100));
}
// EOF

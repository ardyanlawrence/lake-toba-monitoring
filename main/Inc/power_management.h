/*
 *  power_management.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/


#ifndef __POWER_MANAGEMENT_H
#define __POWER_MANAGEMENT_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "driver/rtc_io.h"
#include "soc/sens_periph.h"
#include "soc/rtc.h"

#include "adc_app.h"
#include "board.h"

typedef enum{
	USING_BATTERY = 0,
	USING_ADAPTOR
} power_stat_t;

float battery;
float adaptor;

void power_mng_init(void);
float power_adaptor(void);
float power_battery(void);
power_stat_t power_checking(void);
void set_device_timer_deepsleep(uint32_t seconds);
void set_device_ext0_deepsleep(gpio_num_t gpio_num, int level);
esp_sleep_wakeup_cause_t reset_cause_check(void);

#endif

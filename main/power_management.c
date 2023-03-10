/*
 *  power_management.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

#include "power_management.h"

void power_mng_init(void)
{
	// initialize channels adc where battery and adaptor power measured
	// see readme file for complete pins

	//adc_init(BAT_STAT); // battery stat
	//adc_init(ADAPTOR); // power adaptor stat

}

float power_adaptor(void)
{
	float adaptor;
	// read adc measurement
	//adaptor = adc_read_val(ADAPTOR);
	adaptor = 0.5;
	return adaptor;
}

float power_battery(void)
{
	float batt;
	//batt = adc_read_val(BAT_STAT);
	batt = 3.5;
	return batt;
}

power_stat_t power_checking(void)
{
	power_stat_t status;
	// check power
	battery = 0.00;
	adaptor = 5.00;

	//battery = power_battery()/0.1304;
	//adaptor = power_adaptor()/0.1304;

	printf("batt:%f, adaptor:%f\r\n",battery,adaptor);

	if (adaptor>3.00){
		status = USING_ADAPTOR;
	}
	else{
		status = USING_BATTERY;
	}

	return status;
}

void set_device_timer_deepsleep(uint32_t seconds)
{
	printf("Enabling timer wakeup, %i\n", seconds);
	esp_sleep_enable_timer_wakeup(seconds * 1000000);
	printf("Entering deep sleep\n");
	esp_deep_sleep_start();
}

void set_device_ext0_deepsleep(gpio_num_t gpio_num, int level)
{
	printf("Enabling Exti wakeup interrupt\r\n");
	esp_sleep_enable_ext0_wakeup(gpio_num, level);
	printf("Entering deepsleep\r\n");
	esp_deep_sleep_start();
}

esp_sleep_wakeup_cause_t reset_cause_check(void)
{
	esp_sleep_wakeup_cause_t cause;
	cause = esp_sleep_get_wakeup_cause();
	return cause;
}

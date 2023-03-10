/*
 *  sensors.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 28/3/2022
 * 	revison	:
 *
*/

#ifndef SENSORS_H
#define SENSORS_H

#include "adc_app.h"
#include "gpio_app.h"
#include "modbus_app.h"
#include "sd_card.h"
#include "platform.h"
#include "wifi_app.h"
#include "mqtt_app.h"
#include "ntp_app.h"
#include "digital_input.h"
#include "digital_debounce_input.h"
#include "analog_input.h"
#include "pulse_counter.h"
#include "freq_input.h"
#include "modbus_app.h"

void sensor_run(void);

#endif

//EOF

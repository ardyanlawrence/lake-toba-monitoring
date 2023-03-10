/*
 *  adc_app.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

#ifndef __ADC_APP_H
#define __ADC_APP_H

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   16          //Multisampling

void check_efuse(void);
void print_char_val_type(esp_adc_cal_value_t val_type);
void adc_init(adc_channel_t adc_ch);
float adc_read_val(adc_channel_t acd_ch);
uint32_t adc_read_raw(adc_channel_t acd_ch);

#endif

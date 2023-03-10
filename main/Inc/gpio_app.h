/*
 *  gpio_app.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
 */

#ifndef __GPIO_APP_H
#define __GPIO_APP_H

#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/xtensa_api.h"
#include "freertos/task.h"
#include "board.h"

uint32_t pulse_ch1;
uint32_t pulse_ch2;
uint32_t pulse_ch3;

void gpio_initialize(gpio_num_t pin_num, gpio_mode_t mode, gpio_int_type_t int_type, gpio_pulldown_t pulldown_en, gpio_pullup_t pullup_en);
void gpio_set_interrupt(uint32_t pin_num);
void gpio_do_counting(uint32_t io_num);

#endif

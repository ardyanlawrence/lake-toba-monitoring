/*
 *  digital_input.h file
 *
 * 	author	: Iqbal
 * 	email	: iqbalfauzan29@gmail.com
 * 	date	: 14/02/2023
 * 	revison	: 
 *
*/

#ifndef DIGITAL_DEBOUNCE_INPUT
#define DIGITAL_DEBOUNCE_INPUT

#include "gpio_app.h"
#include "platform.h"
#include "ntp_app.h"

void digital_debounce_input_init(char* settings);
void debounce_sdcard_save_task(void *arg);
void debounce_publish_task(void *arg);

#endif
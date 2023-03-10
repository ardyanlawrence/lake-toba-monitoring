/*
 *  digital_input.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 30/3/2022
 * 	revison	: 3/1/2023 by Iqbal (iqbalfauzan29@gmail.com)
 *
*/

#ifndef DIGITAL_INPUT
#define DIGITAL_INPUT

#include "board.h"
#include "gpio_app.h"
#include "platform.h"
#include "ntp_app.h"

void digital_input_init(char* settings);
void digital_input_task(void *arg);

#endif

//EOF

/*
 *  analog_input.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 31/3/2022
 * 	revison	: 3/1/2023 by Iqbal (iqbalfauzan29@gmail.com)
 *
*/

#ifndef ANALOG_INPUT
#define ANALOG_INPUT

#include "adc_app.h"
#include "platform.h"
#include "board.h"
#include "ntp_app.h"
#include "sd_card.h"

void analog_input_init(char* settings);
void analog_input_task(void *arg);

#endif

//EOF


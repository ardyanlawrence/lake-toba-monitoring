/*
 *  freq_input.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 4/4/2022
 * 	revison	:
 *
*/


#ifndef FREQ_INPUT_H
#define FREQ_INPUT_H

#include "board.h"
#include "gpio_app.h"
#include "platform.h"
#include "ntp_app.h"

void freq_input_init(char* settings);
void freq1_input_task(void *arg);
void freq2_input_task(void *arg);
void freq3_input_task(void *arg);

#endif


//EOF


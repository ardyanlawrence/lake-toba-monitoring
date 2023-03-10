/*
 *  pulse_counter.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 4/4/2022
 * 	revison	:
 *
*/


#ifndef PULSE_COUNTER_H
#define PULSE_COUNTER_H

#include "board.h"
#include "gpio_app.h"
#include "platform.h"
#include "ntp_app.h"

void pulse_counter_init(char* settings);
void pulse1_counter_task(void *arg);
void pulse2_counter_task(void *arg);
void pulse3_counter_task(void *arg);

#endif


//EOF


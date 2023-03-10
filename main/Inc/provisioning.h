/*
 *  provisioning.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 23/2/2022
 * 	revison	:
 *
*/


#ifndef __PROVISIONING_H
#define __PROVISIONING_H

#include "board.h"
#include "ble_server.h"
#include "wifi_app.h"
#include "sd_card.h"

void provisioning_device (void);
bool provisioning_status(void);
void parse_prov_setting(char* settings, int len);

#endif
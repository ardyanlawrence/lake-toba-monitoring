/*
 *  eeprom_app.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/



#ifndef __EEPROM_APP_H
#define __EEPROM_APP_H

#include "i2c_app.h"

void eeprom_init(void);
void eeprom_pwr_on(void);
void eeprom_pwr_off(void);
int eeprom_write_data(uint16_t reg_addr, uint8_t* data, size_t data_len);
int eeprom_read_data(uint16_t reg_addr, uint8_t* data, size_t data_len);

#endif

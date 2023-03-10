/*
 *  eeprom_app.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

/*
 *  EEPROM APPLICATION PROGRAMM
 */

#include "eeprom_app.h"

void eeprom_init(void)
{
	i2c_init(I2C_NUM,I2C_SDA,I2C_SCL,100000);
}

/*
 *  brief:
 *  reg addr -> word eeprom register page
 *  data -> data want to store in bytes, 1 page has 64 bytes length
 *  data_len -> lenght of data
 *
 */

int eeprom_write_data(uint16_t reg_addr, uint8_t* data, size_t data_len)
{
	int status = 0;
	esp_err_t i2c_stat;

	i2c_stat = i2c_write(I2C_NUM, reg_addr, data, data_len);
	if (i2c_stat != ESP_OK){
		status = 0;
		//printf("failed to write\r\n");
	}
	else {
		status = 1;
		//printf("eeprom write OK\r\n");
	}

	return status;
}


int eeprom_read_data(uint16_t reg_addr, uint8_t* data, size_t data_len)
{
	int status = 0;
	esp_err_t i2c_stat;

	i2c_stat = i2c_read(I2C_NUM, reg_addr, data, data_len);
	if (i2c_stat != ESP_OK){
		status = 0;
		//printf("failed to read\r\n");
	}
	else {
		status = 1;
		//printf("eeprom read OK\r\n");
	}

	return status;
}

void eeprom_pwr_on(void)
{
	//switch_on_eeprom_pwr();
}

void eeprom_pwr_off(void)
{
	//switch_off_eeprom_pwr();
}

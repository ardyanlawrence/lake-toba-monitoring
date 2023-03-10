/*
 *  i2c_app.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

#ifndef __I2C_APP_H
#define __I2C_APP_H

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "board.h"

#define I2C_MASTER_TX_BUF_DISABLE 	0             /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE	0
#define ACK_CHECK_EN 				0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 				0x0           /*!< I2C master will not check ack from slave */
#define ACK_VAL 					0x0           /*!< I2C ack value */
#define NACK_VAL 					0x1           /*!< I2C nack value */
#define I2C_NUM 					0

esp_err_t i2c_init(i2c_port_t i2c_num, int i2c_sda_pin, int i2c_scl_pin, int frequency);
esp_err_t i2c_write(i2c_port_t i2c_num, uint16_t reg_addr, uint8_t *data_wr, size_t size);
esp_err_t i2c_read(i2c_port_t i2c_num, uint16_t reg_addr, uint8_t *data_rd, size_t size);

#endif


/*
 *  sd_card.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/


#ifndef __SD_CARD_H
#define __SD_CARD_H

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "sdkconfig.h"
#include <dirent.h>
#include "board.h"
#include <unistd.h>

void sdcard_init(gpio_num_t mosi, gpio_num_t miso, gpio_num_t sck, gpio_num_t cs); // init sdcard
//void sdcard_init(void);
void sdcard_mkdir(char* dirname);
// filename max len is 5 bytes
void sdcard_write_file(char* folder, char* filename, char *data);
bool sdcard_check_file_exist(char* folder, char* filename);
void sdcard_read_file(char* folder, char* filename, char *data);
void sdcard_read_backup(char* folder, char* filename, char *data);
void sdcard_remove_file(char* filename);
void sdcard_remove_in_folder(char* folder);
int sdcard_countfile(char* folder);

#endif

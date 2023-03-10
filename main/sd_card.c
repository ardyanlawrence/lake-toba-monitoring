/*
 *  sd_card.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

/*
CS# = SPICS0 = CS = GPIO10
DI = SPID = CMD = MOSI = GPIO11
DO = SPIQ = Dat0 = MISO = GPIO13
CLK = SPICLK = CLK = GPIO12
 *
 */

#include "sd_card.h"

static const char *TAG = "example";
const char FOLDER_BACKUP[] = "Backup";
const char FOLDER_SETTING[] = "setting";
#define MOUNT_POINT "/sdcard"
#define SPI_DMA_CHAN    host.slot
// for ESP32 C3
//#define SPI_DMA_CHAN    SPI_DMA_CH_AUTO

SemaphoreHandle_t sdcard_write_file_sem = NULL;
SemaphoreHandle_t sdcard_read_file_sem = NULL;
SemaphoreHandle_t sdcard_read_backup_sem = NULL;
SemaphoreHandle_t sdcard_remove_sem = NULL;
SemaphoreHandle_t sdcard_countfile_sem = NULL;


void sdcard_init(gpio_num_t mosi, gpio_num_t miso, gpio_num_t sck, gpio_num_t cs)
{
	esp_err_t ret;
	esp_vfs_fat_sdmmc_mount_config_t mount_config;
	mount_config.format_if_mount_failed = false;
	mount_config.max_files = 5;
	mount_config.allocation_unit_size = 16 * 1024;
	sdmmc_card_t* card;
	const char mount_point[] = MOUNT_POINT;
	ESP_LOGI(TAG, "Initializing SD card");
	ESP_LOGI(TAG, "Using SPI peripheral");

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	host.max_freq_khz = 5000;
	
	spi_bus_config_t bus_cfg = {
	        .mosi_io_num = mosi,
	        .miso_io_num = miso,
	        .sclk_io_num = sck,
	        .quadwp_io_num = -1,
	        .quadhd_io_num = -1,
	        .max_transfer_sz = 4000,
	};

	ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHAN);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Failed to initialize bus.");
	}

	    // This initializes the slot without card detect (CD) and write protect (WP) signals.
	    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
	sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
	slot_config.gpio_cs = cs;
	slot_config.host_id = host.slot;

	ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

	while (ret != ESP_OK){
		ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);
		if (ret == ESP_FAIL) {
			ESP_LOGE(TAG, "Failed to mount filesystem. If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
	    } else {
	        ESP_LOGE(TAG, "Failed to initialize the card (%s). Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
	    }
	}

	// Card has been initialized, print its properties
	sdmmc_card_print_info(stdout, card);

/*
	    // All done, unmount partition and disable SDMMC or SPI peripheral
	    esp_vfs_fat_sdcard_unmount(mount_point, card);
	    ESP_LOGI(TAG, "Card unmounted");
	    spi_bus_free(host.slot);
	    */

	sdcard_write_file_sem = xSemaphoreCreateMutex();
	sdcard_read_file_sem = xSemaphoreCreateMutex();
	sdcard_read_backup_sem = xSemaphoreCreateMutex();
	sdcard_remove_sem = xSemaphoreCreateMutex();
	sdcard_countfile_sem = xSemaphoreCreateMutex();

}

void sdcard_mkdir(char* dirname)
{
	char folder_name[32];

	ESP_LOGI(TAG, "Creating Directory");
	sprintf(folder_name, "%s/%s",MOUNT_POINT,dirname);
    fflush(stdout);
    // check if directory backup is exist
    printf("foldername: %s\r\n",folder_name);
    DIR* d = opendir(folder_name);
    if (!d)
    {
        mkdir(folder_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        printf("directory not found\r\n");
        // create directory
    }
    fflush(stdout);
}

// filename max len is 5 bytes
void sdcard_write_file(char* folder, char* filename, char *data)
{
	if( xSemaphoreTake(sdcard_write_file_sem, (TickType_t) 500)){
		char file_name[128];
		//printf("write data\r\n");
		sprintf(file_name, "%s/%s/%s.txt",MOUNT_POINT,folder,filename);
		//printf("%s\r\n",file_name);

		fflush(stdout);
		FILE*   f = fopen(file_name, "w+");
		if (!f) {
				// Create the numbers file if it doesn't exist
				//printf("failed to open\r\n");
		}

		fflush(stdout);
		fprintf(f,"%s",data);
		fflush(stdout);
		fclose(f);

		xSemaphoreGive(sdcard_write_file_sem);
	}
}

bool sdcard_check_file_exist(char* folder, char* filename)
{
	char file_name[128];
	sprintf(file_name,"%s/%s/%s.txt",MOUNT_POINT,folder,filename);
	printf("checking filename:%s\r\n",file_name);
	if (access(file_name, F_OK) == 0) {
		printf("%s exists\r\n",file_name);
		return true;
	} else {
		printf("%s doesn't exist\r\n",file_name);
	    return false;
	}
}

void sdcard_read_file(char* folder, char* filename, char *data)
{
	if( xSemaphoreTake(sdcard_read_file_sem, (TickType_t) 500)){
		char file_name[128];
		sprintf(file_name,"%s/%s/%s.txt",MOUNT_POINT,folder,filename);
		// printf("filename:%s\r\n",file_name);
		fflush(stdout);
		FILE* f=fopen(file_name,"r+");

		if (!f){
			printf("file not found\r\n");
		}
		else{
			fflush(stdout);
			// load setting to buffer
			fscanf(f,"%[^\n]%*c",data);
			fflush(stdout);
			fclose(f);
			fflush(stdout);
		}

		xSemaphoreGive(sdcard_read_file_sem);
	}
}


void sdcard_read_backup(char* folder, char* filename, char *data)
{
	if( xSemaphoreTake(sdcard_read_backup_sem, (TickType_t) 500)){
		// open folder and scan data
		char folder_name[64];

		//printf("read backup data\r\n");
		// open folder first to read file inside it
		sprintf(folder_name,"%s/%s",MOUNT_POINT,folder);
		fflush(stdout);
		DIR* d = opendir(folder_name);
		struct dirent* e = readdir(d);

		if(e){
			//printf("file found\r\n");
			sprintf(filename,"%s/%s/%s",MOUNT_POINT,folder,e->d_name);
			// now read detected file
			fflush(stdout);
			FILE*   f = fopen(filename, "r+");
			fflush(stdout);
			printf("read data from file %s\r\n", filename);
			fscanf(f,"%s",data);
			fflush(stdout);
			fclose(f);
		// close folder
		}
		else {
			printf("file not found\r\n");
		}
		fflush(stdout);
		closedir(d);
		fflush(stdout);

		xSemaphoreGive(sdcard_read_backup_sem);
	}
}


void sdcard_remove_file(char* filename)
{
	if( xSemaphoreTake(sdcard_remove_sem, (TickType_t) 500)){
		// erase data
		fflush(stdout);
		//printf("Removing Data: %s\r\n",filename);
		//sprintf(filename,"/fs/%s/%s",foldername,e->d_name);
		remove(filename);

		xSemaphoreGive(sdcard_remove_sem);
	}
}

void sdcard_remove_in_folder(char* folder)
{
	char folder_name[32];
	char filename[512];

    printf("read data\r\n");
    // open folder first to read file inside it
    sprintf(folder_name,"%s/%s",MOUNT_POINT,folder);
    fflush(stdout);
    DIR* d = opendir(folder_name);
    //struct dirent* e = readdir(d);
	struct dirent* e;

	while(1){
		e = readdir(d);
		if(e){
			printf("file found\r\n");
			sprintf(filename,"%s/%s/%s",MOUNT_POINT,folder,e->d_name);
			printf("filename:%s\r\n",filename);
			// remove file
			sdcard_remove_file(filename);
			fflush(stdout);
			// close folder
			}
		else {
			printf("file not found\r\n");
			break;
		}
	}
    fflush(stdout);
    closedir(d);
    fflush(stdout);
}

int sdcard_countfile(char* folder)
{
	int amount = 0;
	if( xSemaphoreTake(sdcard_countfile_sem, (TickType_t) 500)){
		char folder_name[32];
		sprintf(folder_name,"%s/%s",MOUNT_POINT,folder);
		fflush(stdout);
		DIR* d = opendir(folder_name);
		while (true) {
			struct dirent*  e = readdir(d);

			if (!e) {
				break;
			}
			amount++;
			//printf("    %s\n", e->d_name);
		}

		//printf("amount of file: %i \r\n",amount);
		xSemaphoreGive(sdcard_countfile_sem);
		
	}
	return amount;
}

//EOF

/*
 *  digital_debounce_input.c file
 *
 * 	author	: Iqbal
 * 	email	: iqbalfauzan29@gmail.com
 * 	date	: 14/02/2023
 * 	revison	: 
 *
 */

#include "digital_debounce_input.h"

SemaphoreHandle_t debounce_semaphore_1;
SemaphoreHandle_t debounce_semaphore_2;
SemaphoreHandle_t debounce_semaphore_3;
QueueHandle_t debounce_queue_1;
QueueHandle_t debounce_queue_2;
QueueHandle_t debounce_queue_3;


void IRAM_ATTR debounce_isr_handler(void* arg) 
{
    uint32_t dig_ch = (uint32_t) arg;
    gpio_intr_disable(dig_ch);
    bool io_lvl = gpio_get_level(dig_ch);
    if (dig_ch == DIGITAL_CH1)
    {
        xQueueSendFromISR(debounce_queue_1, &io_lvl, NULL);
    }
    else if (dig_ch == DIGITAL_CH2)
    {
        xQueueSendFromISR(debounce_queue_2, &io_lvl, NULL);
    }
    else if (dig_ch == DIGITAL_CH3)
    {
        xQueueSendFromISR(debounce_queue_3, &io_lvl, NULL);
    }
}

void digital_debounce_input_init(char *settings)
{
    printf("running digital sensor with debounce\r\n");
    static uint8_t task_i = 1;
    char th_name[32];
    sprintf(th_name, "DBI_TASK%i", task_i);
    task_i++;
    xTaskCreate(debounce_sdcard_save_task, th_name, DEFAULT_SENSOR_STACK_SIZE, settings, GPIO_HANDLER_PRIORITY, NULL);
}

void debounce_sdcard_save_task(void *arg)
{
    char io;
    char sensor_name[32];
    char param_name[32];
    int wait;
    char *msg = (char *)arg;
    bool is_active_low;

    sscanf(strstr(msg, "io"), "io\":%c", &io);
    // sscanf(strstr(msg, "active_low"), "active_low\":%c", &is_active_low);
    sscanf(strstr(msg, "wait"), "wait\":%d", &wait);
    sscanf(strstr(msg, "sensor"), "sensor\":%[^}, ]", sensor_name);
    sscanf(strstr(msg, "param"), "param\":%[^}, ]", param_name);
    printf("io:%c, wait:%d, sensor:%s, param:%s\r\n", 
        io, wait, sensor_name, param_name);

    uint32_t dig_ch = 0;
    char backup_folder[8];
    QueueHandle_t queue;
    SemaphoreHandle_t semaphore;
    if (io == '1')
    {
        dig_ch = DIGITAL_CH1;
        strcpy(backup_folder, DIGITAL1_BACKUP_FOLDER);
        debounce_queue_1 = xQueueCreate(32, sizeof(bool));
        queue = debounce_queue_1;
        debounce_semaphore_1 = xSemaphoreCreateBinary();
        semaphore = debounce_semaphore_1;
    }
    else if (io == '2')
    {
        dig_ch = DIGITAL_CH2;
        strcpy(backup_folder, DIGITAL2_BACKUP_FOLDER);
        debounce_queue_2 = xQueueCreate(32, sizeof(bool));
        queue = debounce_queue_2;
        debounce_semaphore_2 = xSemaphoreCreateBinary();
        semaphore = debounce_semaphore_2;
    }
    else // if (io == '3')
    {
        dig_ch = DIGITAL_CH3;
        strcpy(backup_folder, DIGITAL3_BACKUP_FOLDER);
        debounce_queue_3 = xQueueCreate(32, sizeof(bool));
        queue = debounce_queue_3;
        debounce_semaphore_3 = xSemaphoreCreateBinary();
        semaphore = debounce_semaphore_3;
    }

    char publish_th_name[16];
    sprintf(publish_th_name, "DBI_PUB_TASK%d", io);
    xTaskCreate(debounce_publish_task, publish_th_name, DEFAULT_SENSOR_STACK_SIZE, backup_folder, tskIDLE_PRIORITY, NULL);

    gpio_initialize(dig_ch, GPIO_MODE_INPUT, GPIO_INTR_ANYEDGE, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_DISABLE);
    gpio_isr_handler_add(dig_ch, debounce_isr_handler, (void*)dig_ch);
    while(1)
    {
        bool io_isr_lvl;
        xQueueReceive(queue, &io_isr_lvl, portMAX_DELAY);
        printf("interrupt detected at io %c\r\n", io);
        vTaskDelay(pdMS_TO_TICKS(wait));
        bool io_lvl_after_wait = gpio_get_level(dig_ch);
        gpio_intr_enable(dig_ch);
        printf("io isr level:%d, io level after waiting %d ms:%d\r\n", io_isr_lvl, wait, io_lvl_after_wait);
        if (io_isr_lvl != io_lvl_after_wait) 
        {
            printf("io level changed, isr ignored\r\n");
            continue;
        }
        printf("saving data to sdcard\r\n");

        // Pack to payload
        int64_t timestamp = ntp_to_epoch() / 1000;
        char buffer[512];
        bzero(buffer, sizeof(buffer));
        sprintf(buffer, "{\"ts\":%llu,\"sensor\":%s,\"param\":%s,\"value\":%s}", 
            timestamp, sensor_name, param_name, io_isr_lvl ? "true" : "false");
        printf("buffer payload: %s\r\n", buffer);

        // save to sdcard
        char backup_file[10];
        sprintf(backup_file, "%d", (int32_t)(timestamp % 1000000));
        printf("filename: %s\r\n", backup_file);
        sdcard_write_file(backup_folder, backup_file, buffer);

        xSemaphoreGive(semaphore);
    }   
}

void debounce_publish_task(void *arg)
{
    char *backup_folder = (char *)arg;
    SemaphoreHandle_t semaphore;
    if (strcmp(backup_folder, DIGITAL1_BACKUP_FOLDER) == 0)
    {
        semaphore = debounce_semaphore_1;
    }
    else if (strcmp(backup_folder, DIGITAL2_BACKUP_FOLDER) == 0)
    {
        semaphore = debounce_semaphore_2;
    }
    else // if (io == '3')
    {
        semaphore = debounce_semaphore_3;
    }

    while(1)
    {
        printf("waiting for semaphore before publishing\r\n");
        xSemaphoreTake(semaphore, pdMS_TO_TICKS(60000)); // timeout: 1 minutes        

        // check data in backup folder
        int release_stat = true;
        while (release_stat > 0)
        {
            printf("try publishing files from %s\r\n", backup_folder);
            release_stat = release_procedure(backup_folder);
            printf("publishing status: %d\r\n\r\n", release_stat);
        }
    }
}
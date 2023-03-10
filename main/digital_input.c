/*
 *  digital_input.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 30/3/2022
 * 	revison	: 3/1/2023 by Iqbal (iqbalfauzan29@gmail.com)
 *
 */

#include "digital_input.h"

void digital_input_init(char *settings)
{
    printf("running digital sensor\r\n");
    static uint8_t task_i = 1;
    char th_name[32];
    sprintf(th_name, "DI_TASK%i", task_i);
    task_i++;
    xTaskCreate(digital_input_task, th_name, DEFAULT_SENSOR_STACK_SIZE, settings, tskIDLE_PRIORITY, NULL);
}

////////// TASK //////////////////
void digital_input_task(void *arg)
{
    char IO;
    char sensor_name[32];
    char param_name[32];
    int sampling; // interval in second
    char *msg = (char *)arg;

    sscanf(strstr(msg, "sampling"), "sampling\":%d", &sampling);
    sscanf(strstr(msg, "io"), "io\":%c", &IO);
    sscanf(strstr(msg, "sensor"), "sensor\":%[^}, ]", sensor_name);
    sscanf(strstr(msg, "param"), "param\":%[^}, ]", param_name);
    printf("IO:%c, sampling:%d, sensor:%s, param:%s\r\n", 
        IO, sampling, sensor_name, param_name);

    uint32_t dig_ch = 0;
    char backup_folder[8];
    switch (IO)
    {
    case '1':
        dig_ch = DIGITAL_CH1;
        strcpy(backup_folder, DIGITAL1_BACKUP_FOLDER);
        break;

    case '2':
        dig_ch = DIGITAL_CH2;
        strcpy(backup_folder, DIGITAL2_BACKUP_FOLDER);
        break;

    case '3':
        dig_ch = DIGITAL_CH3;
        strcpy(backup_folder, DIGITAL3_BACKUP_FOLDER);
        break;

    default:
        break;
    }

    gpio_initialize(dig_ch, GPIO_MODE_INPUT, GPIO_INTR_DISABLE, GPIO_PULLDOWN_DISABLE, GPIO_PULLUP_DISABLE);
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (1)
    {
        printf("digital %c input task \r\n", IO);

        int IO_Value = gpio_get_level(dig_ch);

        // Pack to platform payload
        int64_t timestamp = ntp_to_epoch() / 1000;
        char buffer[512];
        bzero(buffer, sizeof(buffer));
        sprintf(buffer, "{\"ts\":%llu,\"sensor\":%s,\"param\":%s,\"value\":%s}", 
            timestamp, sensor_name, param_name, IO_Value ? "true" : "false");
        printf("buffer payload: %s\r\n", buffer);

        // save to sdcard
        char backup_file[8];
        sprintf(backup_file, "%i", (uint16_t)(timestamp / 1000));
        printf("filename: %s\r\n", backup_file);
        sdcard_write_file(backup_folder, backup_file, buffer);

        // check data in backup folder
        int filenum = sdcard_countfile(backup_folder);
        for (int i = 0; i < filenum; i++)
        {
            // publish
            release_procedure(backup_folder);
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(sampling*1000)); // delay
    }
}
// EOF
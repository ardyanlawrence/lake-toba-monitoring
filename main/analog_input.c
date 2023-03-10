/*
 *  analog_input.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 31/3/2022
 * 	revison	: 3/1/2023 by Iqbal (iqbalfauzan29@gmail.com)
 *
 */

#include "analog_input.h"

void analog_input_init(char *settings)
{
    printf("running analog sensor\r\n");
    static uint8_t task_i = 1;
    char th_name[32];
    sprintf(th_name, "AN_TASK%i", task_i);
    task_i++;
    xTaskCreate(analog_input_task, th_name, DEFAULT_SENSOR_STACK_SIZE, settings, tskIDLE_PRIORITY, NULL);
}

////////// TASK //////////////////
void analog_input_task(void *arg)
{
    char IO;
    char sensor_name[32];
    char param_name[32];
    int sampling;
    char mode = '0';
    int min_val, max_val, outrange_period, inrange_period;
    char *msg = (char *)arg;

    if (strstr(msg, "threshold")) // instant
    {
        mode = 'I';
        sscanf(strstr(msg, "min_val"), "min_val\":%d", &min_val);
        sscanf(strstr(msg, "max_val"), "max_val\":%d", &max_val);
        sscanf(strstr(msg, "outrange_period"), "outrange_period\":%d", &outrange_period);
        sscanf(strstr(msg, "inrange_period"), "inrange_period\":%d", &inrange_period);
        printf("min_val:%d, max_val:%d, outrange_period:%d, inrange_period:%d\r\n",
               min_val, max_val, outrange_period, inrange_period);
    }
    else // sampling
    {
        mode = 'S';
        sscanf(strstr(msg, "sampling"), "sampling\":%d", &sampling);
        printf("sampling:%d\r\n", sampling);
    }
    sscanf(strstr(msg, "io"), "io\":%c", &IO);
    sscanf(strstr(msg, "sensor"), "sensor\":%[^}, ]", sensor_name);
    sscanf(strstr(msg, "param"), "param\":%[^}, ]", param_name);
    printf("mode: %c, IO:%c, sensor:%s, param:%s\r\n",
           mode, IO, sensor_name, param_name);

    uint32_t an_ch = 0;
    char backup_folder[8];
    switch (IO)
    {
    case '1':
        an_ch = ANALOG_CH1;
        strcpy(backup_folder, ANALOG1_BACKUP_FOLDER);
        break;

    case '2':
        an_ch = ANALOG_CH2;
        strcpy(backup_folder, ANALOG2_BACKUP_FOLDER);
        break;

    case '3':
        an_ch = ANALOG_CH3;
        strcpy(backup_folder, ANALOG3_BACKUP_FOLDER);
        break;

    default:
        break;
    }

    adc_init(an_ch);
    TickType_t lastWakeTime = xTaskGetTickCount();
    while (1)
    {
        printf("analog %c input task \r\n", IO);

        int AN_Value = adc_read_raw(an_ch);

        // Pack to platform payload
        int64_t timestamp = ntp_to_epoch() / 1000;
        char buffer[512];
        bzero(buffer, sizeof(buffer));
        sprintf(buffer, "{\"ts\":%llu,\"sensor\":%s,\"param\":%s,\"value\":%i}",
                timestamp, sensor_name, param_name, AN_Value);
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

        // delay
        unsigned long interval;
        if (mode == 'I') // instant
        {
            bool is_inrange = (min_val <= AN_Value) && (AN_Value <= max_val);
            if (is_inrange)
            {
                interval = inrange_period * 1000;
                printf("interval:%lu\r\n", interval);
                while ((xTaskGetTickCount() - lastWakeTime) < pdMS_TO_TICKS(interval) && is_inrange)
                {
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    AN_Value = adc_read_raw(an_ch);
                    is_inrange = (min_val <= AN_Value) && (AN_Value <= max_val);
                }
                lastWakeTime = xTaskGetTickCount();
                continue;
            }
            else
            {
                interval = outrange_period * 1000;
            }
        }
        else // sampling
        {
            interval = sampling * 1000;
        }
        printf("interval:%lu\r\n", interval);
        
        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(interval));
    }
}
// EOF
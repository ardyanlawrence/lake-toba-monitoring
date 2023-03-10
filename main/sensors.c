/*
 *  sensors.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 28/3/2022
 * 	revison	:
 *
 */

#include "sensors.h"

void sensor_run(void)
{
    int sensor_count = sdcard_countfile(SENSOR_SETTINGS_FOLDER);
    printf("sensor count: %i\r\n", sensor_count);
    for (int i = 0; i < sensor_count; i++)
    {
        printf("\r\n");
        char sensor_properties[1024];
        char file_name[256];
        bzero(sensor_properties, sizeof(sensor_properties));
        bzero(file_name, sizeof(file_name));
        sdcard_read_backup(SENSOR_SETTINGS_FOLDER, file_name, sensor_properties);
        sdcard_remove_file(file_name);
        if (strstr(sensor_properties, "ana_i"))
        {
            analog_input_init(sensor_properties);
        }
        else if (strstr(sensor_properties, "dig_i"))
        {
            if (strstr(sensor_properties, "debounce"))
            {
                digital_debounce_input_init(sensor_properties);
            }
            else
            {
                digital_input_init(sensor_properties);
            }
        }
        else if (strstr(sensor_properties, "counter"))
        {
            printf("running counter sensor\r\n");
            pulse_counter_init(sensor_properties);
        }
        else if (strstr(sensor_properties, "freq"))
        {
            printf("running freq sensor\r\n");
            freq_input_init(sensor_properties);
        }
        else if (strstr(sensor_properties, "modbus"))
        {
            modbus_app_init(sensor_properties);
        }
        else if (strstr(sensor_properties, "uart"))
        {
            printf("running uart sensor\r\n");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

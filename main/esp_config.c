/*
 *  esp_config.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 11/3/2022
 * 	revison	:
 *
 */

#include "esp_config.h"

/*
 *  load configuration on sdcard
 *  ID
 *  Wifi & pass
 *  Mqtt username and pass
 *  dll
 */

void load_configuration(void)
{
    // char all_setting[8192];
    char *all_setting = (char *)malloc(5000 * sizeof(char));
    mqtt_load_setting();
    // bzero(all_setting,sizeof(all_setting));
    sdcard_read_file(SETTINGS_FOLDER, "sensors", all_setting);
    // sdcard_read_file(SETTINGS_FOLDER, "sstat", all_setting);
    config_sensornode(all_setting);
}

// get setting from sdcard
void mqtt_load_setting(void)
{
    printf("mqtt load settings \r\n");
    sdcard_read_file(PROVISIONING_FOLDER, "uid", mqtt_uid);
    sdcard_read_file(PROVISIONING_FOLDER, "user", mqtt_username);
    sdcard_read_file(PROVISIONING_FOLDER, "pass", mqtt_password);
    printf("uid: %s, user: %s, password: %s\r\n\r\n", mqtt_uid, mqtt_username, mqtt_password);
}

void config_sensornode(char *settings)
{
    cJSON *all_sett = cJSON_Parse(settings);
    printf("all_setting (from platform): %s\r\n\r\n", settings);
    free(settings);

    // get sampling
    cJSON *sampling = cJSON_GetObjectItem(all_sett, "sampling");

    // add instant to sampling (S#0)
    cJSON *instant = cJSON_GetObjectItem(all_sett, "instant");
    cJSON *instant_copy = cJSON_Duplicate(instant, 1);
    if (cJSON_GetArraySize(instant_copy) != 0)
    {
        cJSON_AddItemToObject(sampling, "S#0", instant_copy);
    }

    // get sensor data
    cJSON *sensors = cJSON_GetObjectItem(all_sett, "sensors");

    char *sampling_print = cJSON_Print(sampling);
    char *sensors_print = cJSON_Print(sensors);

    cJSON_Delete(all_sett);

    parse_sensor_settings(sampling_print, sensors_print);
}

void parse_sensor_settings(char *sampling_settings, char *sensors_data)
{
    char *str_ptr = strpbrk(sampling_settings, "{");
    while(str_ptr != NULL)
    {
        float sampling_rate;
        str_ptr = strpbrk(str_ptr, "S#");
        if (str_ptr == NULL) continue;
        sscanf(str_ptr, "S#%f", &sampling_rate);
        str_ptr = strpbrk(str_ptr, "[");
        while (str_ptr[0] != ']')
        {
            char sensor_name[20];
            str_ptr = strpbrk(str_ptr, "\"");
            sscanf(str_ptr, "\"%[^\"]", sensor_name);
            str_ptr = strpbrk(str_ptr, ",]");

            // check json object
            if (!strstr(sensors_data, sensor_name)) continue;
            char buffer_sensors[5000];
            bzero(buffer_sensors, sizeof(buffer_sensors));
            clean_data_sdcard(sensors_data, buffer_sensors);
            cJSON *sensors_obj = cJSON_Parse(buffer_sensors);

            // parse sensor name to payload name
            char sensor_platform[128];
            char config_platform[128];
            char param_platform[128];
            bzero(sensor_platform, sizeof(sensor_platform));
            bzero(config_platform, sizeof(config_platform));
            bzero(param_platform, sizeof(param_platform));
            sscanf(sensor_name, "%[^:]:%[^:]:%s", sensor_platform, config_platform, param_platform);

            cJSON *sample = cJSON_CreateNumber(sampling_rate);
            cJSON *sensor_platform_name = cJSON_CreateString(sensor_platform);
            cJSON *config_platform_name = cJSON_CreateString(config_platform);
            cJSON *param_platform_name = cJSON_CreateString(param_platform);
            // get object value
            cJSON *sens_prop = cJSON_GetObjectItem(sensors_obj, sensor_name);
            cJSON_AddItemToObject(sens_prop, "sampling", sample);
            cJSON_AddItemToObject(sens_prop, "sensor", sensor_platform_name);
            cJSON_AddItemToObject(sens_prop, "config", config_platform_name);
            cJSON_AddItemToObject(sens_prop, "param", param_platform_name);

            char *sens_print = cJSON_Print(sens_prop);
            cJSON_Delete(sensors_obj);
            // create file settings in sdcard
            char buffer[512];
            bzero(buffer, sizeof(buffer));
            clean_data_sdcard(sens_print, buffer);
            static uint8_t k = 0;
            k++;
            char file_name[64];
            bzero(file_name, sizeof(file_name));
            sprintf(file_name, "set_%i", k);
            printf("Saving to %s in SD card: %s\r\n\r\n", file_name, buffer);
            sdcard_write_file(SENSOR_SETTINGS_FOLDER, file_name, buffer);
        }
    }
}

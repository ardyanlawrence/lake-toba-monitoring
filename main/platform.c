/*
 *  platform.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/3/2022
 * 	revison	:
 *
 */

#include "platform.h"

// MQTT
const char mqtt_url[] = "mqtt://mqtt.iotera.io";
uint32_t mqtt_port = 1883;

/*
 * Must be online before entering this procedure
 * Use WiFi MAC to obtain application_id and device_id
 * Output is sensor config that is saved to SD Card
 */
void platform_settings_device(bool ps_stat)
{
    uint8_t wifi_mac[6];
    char *uid;
    char *mqtt_username;
    char *mqtt_password;

    uid = (char *)malloc(sizeof(char) * 13);

    // create user and pass for MQTT_VERSION uid
    mqtt_username = (char *)malloc(sizeof(char) * 21);
    mqtt_password = (char *)malloc(sizeof(char) * 65);
    esp_wifi_get_mac(WIFI_MODE_STA, wifi_mac);
    sprintf(uid, "%02x%02x%02x%02x%02x%02x", wifi_mac[0], wifi_mac[1], wifi_mac[2], wifi_mac[3], wifi_mac[4], wifi_mac[5]);
    build_username(uid, mqtt_username);
    build_password(uid, mqtt_password);

    // get application_id and device_id
    mqtt_init(mqtt_url, mqtt_port, mqtt_username, mqtt_password, uid, ps_stat);

    // free(mqtt_username);
    // free(mqtt_password);
    // free(uid);
}

void platform_mqtt_handler(char *mqtt_topic, char *mqtt_data, uint8_t topic_type)
{
    switch (topic_type)
    {
    case 1:
    {
        printf("GOT case 1 mqtt_idquery\r\n");
        // parse application_id and device_id
        cJSON *mqtt_idquery = cJSON_Parse(mqtt_data);
        cJSON *mqtt_application_id = cJSON_GetObjectItem(mqtt_idquery, "application_id");
        cJSON *mqtt_device_id = cJSON_GetObjectItem(mqtt_idquery, "device_id");
        char *tmp_application_id = cJSON_Print(mqtt_application_id);
        char *tmp_device_id = cJSON_Print(mqtt_device_id);
        // write to SD Card
        sdcard_write_file(PROVISIONING_FOLDER, "psstat", "1");
        remove_quotes(tmp_application_id);
        remove_quotes(tmp_device_id);
        sdcard_write_file(PROVISIONING_FOLDER, "appid", tmp_application_id);
        sdcard_write_file(PROVISIONING_FOLDER, "devid", tmp_device_id);
        cJSON_free(tmp_application_id);
        cJSON_free(tmp_device_id);
        esp_restart();
        break;
    }
    case 2:
    {
        printf("GOT case 2 mqtt_sensorsquery\r\n");
        // get sensors config
        heap_caps_check_integrity_all(true);
        sdcard_write_file(SETTINGS_FOLDER, "sensors", mqtt_data);
        sdcard_write_file(PROVISIONING_FOLDER, "sqstat", "1");
    }
    default:
        break;
    }
}

void remove_char(char *payload, char target_char, int len_payload, char *result)
{
    // printf ("len_payload: %i\r\n",len_payload);
    // printf ("target char: %c\r\n",target_char);
    // printf ("result: ");
    for (int i = 0; i < len_payload; i++)
    {
        if (*payload != target_char)
        {
            *result = *payload;
            // printf ("%c",*result);
            result++;
        }
        payload++;
    }
    // printf ("\r\n");
}

void remove_char2(char *payload, char target_char, char *result)
{
    while (*payload != '\0')
    {
        if (*payload != target_char)
        {
            *result = *payload;
            result++;
        }
        payload++;
    }
}

void clean_data(char *payload, int len_payload, char *result)
{
    printf("len_payload: %i\r\n", len_payload);
    printf("result: ");
    for (int i = 0; i < len_payload; i++)
    {
        if ((*payload != 0x0a) && (*payload != 0x09))
        {
            *result = *payload;
            printf("%c%.2x  ", *result, *result);
            result++;
        }
        payload++;
    }
    printf("\r\n");
}

void clean_data_sdcard(char *payload, char *result)
{
    // printf ("result: ");
    while (*payload != '\0')
    {
        if ((*payload != 0x0a) && (*payload != 0x09) && (*payload != 0x20))
        {
            *result = *payload;
            // printf ("%c%.2x  ",*result,*result);
            // printf ("%c",*result);
            result++;
        }
        payload++;
    }
    // printf ("\r\n");
}

int release_procedure(char *foldername)
{
    uint8_t wifi_stat = 0;
    int mqtt_stat = -1;
    int mqtt_pub_stat = -1;
    char backup_buff[512];
    char file_name[64];
    bzero(backup_buff, 512);
    bzero(file_name, 64);

    // check if file exists
    sdcard_read_backup(foldername, file_name, backup_buff);
    if (strcmp(file_name,"") == 0)
    {
        printf("folder %s empty\r\n", foldername);
        return 0;
    }

    // check wifi
    wifi_stat = read_wifi_stat();
    if (wifi_stat == 0)
    {
        printf("wifi disconnected\r\n");
        return -3;
    }
    
    // check mqtt connection
    mqtt_stat = mqtt_conn_stat();
    if (mqtt_stat != ESP_OK)
    {
        printf("disconnected from mqtt server\r\n");
        return -2;
    }

    // try to publish to iotera server
    mqtt_pub_stat = mqtt_publish_iotera(backup_buff);
    if (mqtt_pub_stat <= 0)
    {
        printf("publish failed:%d\r\n", mqtt_pub_stat);
        return -1;
    }

    sdcard_remove_file(file_name); // remove file when succeded
    printf("publish succeeded, file %s removed\r\n", file_name);
    printf("mqtt pub stat: %d\r\n", mqtt_pub_stat);
    return 1;
}

bool platform_settings_status(void)
{
    bool stat;
    char data[8];

    sdcard_read_file(PROVISIONING_FOLDER, "psstat", data);
    printf("psstat data:%s\r\n", data);
    if (strcmp(data, "1") == 0)
    {
        stat = true;
    }
    else
    {
        stat = false;
    }

    return stat;
}

bool sensors_query_status(bool is_online)
{
    bool stat;
    char data[8];

    if (is_online)
    {
        sdcard_read_file(PROVISIONING_FOLDER, "sqstat", data);
        if (strcmp(data, "1") == 0)
        {
            printf("sqstat data:%s\r\n", data);
            stat = true;
        }
        else
        {
            stat = false;
        }
    }
    else
    {
        stat = sdcard_check_file_exist(SETTINGS_FOLDER, "sensors");
    }

    return stat;
}

// EOF

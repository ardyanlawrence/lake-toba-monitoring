/*
 *  provisioning.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 23/2/2022
 * 	revison	:
 *
*/

#include "provisioning.h"

void provisioning_device (void)
{
    printf("provisioning start\r\n");
    // init BLE
    ble_server_init();

    while (connect_wifi == false)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ble_server_read_message();
    }
    printf("writing to SD Card: %s\r\n", total_server_msg);
    sdcard_write_file(PROVISIONING_FOLDER,"wifi",total_server_msg);
    sdcard_write_file(PROVISIONING_FOLDER,"prstat","1");
    bzero(total_server_msg,sizeof(total_server_msg));
    server_msg_len = 0;
    vTaskDelay(pdMS_TO_TICKS(1000));
	ble_server_disable();
    // restart esp
    esp_restart();
}

bool provisioning_status(void)
{
    bool stat;
    char data[8];
    sdcard_read_file(PROVISIONING_FOLDER,"prstat",data);
    printf("data:%s\r\n",data);
    if (strcmp(data,"1") == 0){
        stat = true;
    }
    else{
        stat = false;
    }

    return stat;
}

// sample: wifi:T_LT2,l4nt4idua.
void parse_prov_setting(char* settings, int len)
{
    char header[32];
    int i = 0;
    int j = 0;

    bzero(header,sizeof(header));
    printf("parsing msg, len:%i\r\n",len);

    printf("header:");
    while ((*settings != ':') && (i<len)){
        header[j] = *settings;
        printf("%c",header[j]);
        i++;
        j++;
        settings++;
    }
    printf("\r\n");

    if (strcmp(header,"wifi") == 0){
        settings++;
        i++;
        j=0;
        
        bzero(prov_ssid,sizeof(prov_ssid));
        bzero(prov_pass,sizeof(prov_pass));

        printf("ssid:");
        while((*settings != ',') && (i<len)){
            prov_ssid[j] = *settings;
            printf("%c",prov_pass[j]);
            i++;
            j++;
            settings++;
        }

        settings++;
        i++;
        j=0;
        printf("pass:");
        while((*settings  != '.') && (i<len)){
            prov_pass[j] = *settings ;
            printf("%c",prov_pass[j]);
            i++;
            j++;
            settings++;
        }
        printf("\r\n");
    } else {
        printf("error parsing provisioning header\r\n");
        prov_ssid[0] = 0;
    }
}

//EOF

/*
 *  pulse_counter.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 4/4/2022
 * 	revison	:
 *
*/

#include "freq_input.h"


void freq_input_init(char* settings)
{
    //char* msg_print;
    char* sensor_platform_name;
    char* config_platform_name;
    char* param_platform_name;
    char buffer[512];
    char th_name[32];

    bzero(buffer,sizeof(buffer));
    bzero(th_name,sizeof(th_name));
    // get settings
    cJSON* freq_set = cJSON_Parse(settings);
    cJSON* freq_prop = cJSON_GetObjectItem(freq_set,"freq");
    int IO = cJSON_GetObjectItem(freq_prop,"io")->valueint;
    int period = cJSON_GetObjectItem(freq_prop,"period")->valueint;
    int inst = cJSON_GetObjectItem(freq_prop,"instant")->valueint;
    int sampling = cJSON_GetObjectItem(freq_set,"sampling")->valueint;
    cJSON* sensor_platform = cJSON_GetObjectItem(freq_set,"sensor");
    cJSON* config_platform = cJSON_GetObjectItem(freq_set,"config");
    cJSON* param_platform = cJSON_GetObjectItem(freq_set,"param");

    // print
    printf("io: %i, period:%i, instant: %i, sampling: %i\r\n",IO, period, inst, sampling);
    sensor_platform_name = cJSON_Print(sensor_platform);
    printf("sensor platform: %s\r\n", sensor_platform_name);
    config_platform_name = cJSON_Print(config_platform);
    printf("sensor platform: %s\r\n", config_platform_name);
    param_platform_name = cJSON_Print(param_platform);
    printf("sensor platform: %s\r\n", param_platform_name);

    sprintf(buffer,"%i:%i:%i:%s:%s",IO,period,sampling,sensor_platform_name,param_platform_name);
    sprintf(th_name,"FREQ_TASK%i",IO);
    // initialize GPIO and tasks
    switch (IO){
        case 1: gpio_initialize(DIGITAL_CH1,GPIO_MODE_INPUT,GPIO_INTR_POSEDGE,GPIO_PULLDOWN_ENABLE,GPIO_PULLUP_DISABLE);
                xTaskCreate(freq1_input_task,th_name,DEFAULT_SENSOR_STACK_SIZE,buffer,tskIDLE_PRIORITY,NULL);
        break;

        case 2: gpio_initialize(DIGITAL_CH2,GPIO_MODE_INPUT,GPIO_INTR_POSEDGE,GPIO_PULLDOWN_ENABLE,GPIO_PULLUP_DISABLE);
                xTaskCreate(freq2_input_task,th_name,DEFAULT_SENSOR_STACK_SIZE,buffer,tskIDLE_PRIORITY,NULL);
        break;

        case 3: gpio_initialize(DIGITAL_CH3,GPIO_MODE_INPUT,GPIO_INTR_POSEDGE,GPIO_PULLDOWN_ENABLE,GPIO_PULLUP_DISABLE);
                xTaskCreate(freq3_input_task,th_name,DEFAULT_SENSOR_STACK_SIZE,buffer,tskIDLE_PRIORITY,NULL);
        break;

        default:
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
}

/////////// TASK /////////////
void freq1_input_task(void *arg)
{
    char* msg;
    char IO_num[5];
    char sampling_num[5];
    char periode_num[5];
    char sensor_platform[32];
    char param_platform[32];
    char buffer[512];
    int64_t timestamp = 0;
    char backup_name[8];
    int filenum = 0;
    //int IO;
    int sampling;
    int freq_value;
    //int periode_val;

    msg = (char*) arg;
    printf("task msg: %s\r\n",msg);
    sscanf(msg,"%[^:]:%[^:]:%[^:]:%[^:]:%s", IO_num,periode_num,sampling_num,sensor_platform,param_platform);
    printf("IO:%s, reset_val:%s, sampling:%s, sensor:%s, param:%s\r\n", IO_num,periode_num,sampling_num,sensor_platform,param_platform);
    //IO = atoi(IO_num);
    sampling = atoi(sampling_num);
    //periode_val = atoi(periode_num);

    while(1){
        printf("freq 1 task \r\n");
        // get counter value from payload
        freq_value = pulse_ch1;
        pulse_ch1 = 0;
        // Pack to platform payload
        timestamp = ntp_to_epoch()/1000;
        sprintf(buffer,"{\"ts\":%llu,\"sensor\":%s,\"param\":%s,\"value\":%i}",timestamp, sensor_platform, param_platform, freq_value);
        printf("buffer payload: %s\r\n",buffer);
        // save to sdcard
        sprintf(backup_name,"%i",(uint16_t)(timestamp/1000));
        printf("filename: %s\r\n",backup_name);
        sdcard_write_file(FREQ1_BACKUP_FOLDER,backup_name,buffer);
        // check data in backup folder
        filenum = sdcard_countfile(FREQ1_BACKUP_FOLDER);
            //printf("amount of file: %i\r\n",filenum);
        if (filenum>0){
            for (int i=0;i<filenum;i++){
                //publish 
                release_procedure(FREQ1_BACKUP_FOLDER);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
        }

        bzero(buffer,sizeof(buffer));
        // delay
        for (int i=0; i<sampling; i++){
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void freq2_input_task(void *arg)
{
    char* msg;
    char IO_num[5];
    char sampling_num[5];
    char periode_num[5];
    char sensor_platform[32];
    char param_platform[32];
    char buffer[512];
    int64_t timestamp = 0;
    char backup_name[8];
    int filenum = 0;
    //int IO;
    int sampling;
    int freq_value;
    //int periode_val;

    msg = (char*) arg;
    printf("task msg: %s\r\n",msg);
    sscanf(msg,"%[^:]:%[^:]:%[^:]:%[^:]:%s", IO_num,periode_num,sampling_num,sensor_platform,param_platform);
    printf("IO:%s, reset_val:%s, sampling:%s, sensor:%s, param:%s\r\n", IO_num,periode_num,sampling_num,sensor_platform,param_platform);
    //IO = atoi(IO_num);
    sampling = atoi(sampling_num);
    //periode_val = atoi(periode_num);

    while(1){
        printf("freq 2 task \r\n");
        // get counter value from payload
        freq_value = pulse_ch2;
        pulse_ch2 = 0;
        // Pack to platform payload
        timestamp = ntp_to_epoch()/1000;
        sprintf(buffer,"{\"ts\":%llu,\"sensor\":%s,\"param\":%s,\"value\":%i}",timestamp, sensor_platform, param_platform, freq_value);
        printf("buffer payload: %s\r\n",buffer);
        // save to sdcard
        sprintf(backup_name,"%i",(uint16_t)(timestamp/1000));
        printf("filename: %s\r\n",backup_name);
        sdcard_write_file(FREQ2_BACKUP_FOLDER,backup_name,buffer);
        // check data in backup folder
        filenum = sdcard_countfile(FREQ2_BACKUP_FOLDER);
            //printf("amount of file: %i\r\n",filenum);
        if (filenum>0){
            for (int i=0;i<filenum;i++){
                //publish 
                release_procedure(FREQ2_BACKUP_FOLDER);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
        }

        bzero(buffer,sizeof(buffer));
        // delay
        for (int i=0; i<sampling; i++){
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void freq3_input_task(void *arg)
{
    char* msg;
    char IO_num[5];
    char sampling_num[5];
    char periode_num[5];
    char sensor_platform[32];
    char param_platform[32];
    char buffer[512];
    int64_t timestamp = 0;
    char backup_name[8];
    int filenum = 0;
    //int IO;
    int sampling;
    int freq_value;
    //int periode_val;

    msg = (char*) arg;
    printf("task msg: %s\r\n",msg);
    sscanf(msg,"%[^:]:%[^:]:%[^:]:%[^:]:%s", IO_num,periode_num,sampling_num,sensor_platform,param_platform);
    printf("IO:%s, reset_val:%s, sampling:%s, sensor:%s, param:%s\r\n", IO_num,periode_num,sampling_num,sensor_platform,param_platform);
    //IO = atoi(IO_num);
    sampling = atoi(sampling_num);
    //periode_val = atoi(periode_num);

    while(1){
        printf("freq 3 task \r\n");
        // get counter value from payload
        freq_value = pulse_ch3;
        pulse_ch3 = 0;
        // Pack to platform payload
        timestamp = ntp_to_epoch()/1000;
        sprintf(buffer,"{\"ts\":%llu,\"sensor\":%s,\"param\":%s,\"value\":%i}",timestamp, sensor_platform, param_platform, freq_value);
        printf("buffer payload: %s\r\n",buffer);
        // save to sdcard
        sprintf(backup_name,"%i",(uint16_t)(timestamp/1000));
        printf("filename: %s\r\n",backup_name);
        sdcard_write_file(FREQ3_BACKUP_FOLDER,backup_name,buffer);
        // check data in backup folder
        filenum = sdcard_countfile(FREQ3_BACKUP_FOLDER);
            //printf("amount of file: %i\r\n",filenum);
        if (filenum>0){
            for (int i=0;i<filenum;i++){
                //publish 
                release_procedure(FREQ3_BACKUP_FOLDER);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
        }

        bzero(buffer,sizeof(buffer));
        // delay
        for (int i=0; i<sampling; i++){
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

//EOF


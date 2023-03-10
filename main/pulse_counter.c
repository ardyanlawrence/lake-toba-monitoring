/*
 *  pulse_counter.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 4/4/2022
 * 	revison	:
 *
*/

#include "pulse_counter.h"


void pulse_counter_init(char* settings)
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
    cJSON* pul_set = cJSON_Parse(settings);
    cJSON* pul_prop = cJSON_GetObjectItem(pul_set,"counter");
    int IO = cJSON_GetObjectItem(pul_prop,"io")->valueint;
    int reset = cJSON_GetObjectItem(pul_prop,"reset")->valueint;
    int inst = cJSON_GetObjectItem(pul_prop,"instant")->valueint;
    int sampling = cJSON_GetObjectItem(pul_set,"sampling")->valueint;
    cJSON* sensor_platform = cJSON_GetObjectItem(pul_set,"sensor");
    cJSON* config_platform = cJSON_GetObjectItem(pul_set,"config");
    cJSON* param_platform = cJSON_GetObjectItem(pul_set,"param");

    // print
    printf("io: %i, reset:%i, instant: %i, sampling: %i\r\n",IO, reset, inst, sampling);
    sensor_platform_name = cJSON_Print(sensor_platform);
    printf("sensor platform: %s\r\n", sensor_platform_name);
    config_platform_name = cJSON_Print(config_platform);
    printf("sensor platform: %s\r\n", config_platform_name);
    param_platform_name = cJSON_Print(param_platform);
    printf("sensor platform: %s\r\n", param_platform_name);

    sprintf(buffer,"%i:%i:%i:%s:%s",IO,reset,sampling,sensor_platform_name,param_platform_name);
    sprintf(th_name,"PULSE_TASK%i",IO);
    // initialize GPIO and tasks
    switch (IO){
        case 1: gpio_initialize(DIGITAL_CH1,GPIO_MODE_INPUT,GPIO_INTR_POSEDGE,GPIO_PULLDOWN_ENABLE,GPIO_PULLUP_DISABLE);
                xTaskCreate(pulse1_counter_task,th_name,DEFAULT_SENSOR_STACK_SIZE,buffer,tskIDLE_PRIORITY,NULL);
        break;

        case 2: gpio_initialize(DIGITAL_CH2,GPIO_MODE_INPUT,GPIO_INTR_POSEDGE,GPIO_PULLDOWN_ENABLE,GPIO_PULLUP_DISABLE);
                xTaskCreate(pulse2_counter_task,th_name,DEFAULT_SENSOR_STACK_SIZE,buffer,tskIDLE_PRIORITY,NULL);
        break;

        case 3: gpio_initialize(DIGITAL_CH3,GPIO_MODE_INPUT,GPIO_INTR_POSEDGE,GPIO_PULLDOWN_ENABLE,GPIO_PULLUP_DISABLE);
                xTaskCreate(pulse3_counter_task,th_name,DEFAULT_SENSOR_STACK_SIZE,buffer,tskIDLE_PRIORITY,NULL);
        break;

        default:
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
}

/////////// TASK /////////////
void pulse1_counter_task(void *arg)
{
    char* msg;
    char IO_num[5];
    char sampling_num[5];
    char reset_num[5];
    char sensor_platform[32];
    char param_platform[32];
    char buffer[512];
    int64_t timestamp = 0;
    char backup_name[8];
    int filenum = 0;
    //int IO;
    char cnt_backup_val[16];
    int sampling;
    int reset_val;
    int counter_tmp1;
    int counter_tmp2;
    int counter_value;

    msg = (char*) arg;
    printf("task msg: %s\r\n",msg);
    sscanf(msg,"%[^:]:%[^:]:%[^:]:%[^:]:%s", IO_num,reset_num,sampling_num,sensor_platform,param_platform);
    printf("IO:%s, reset_val:%s, sampling:%s, sensor:%s, param:%s\r\n", IO_num,reset_num,sampling_num,sensor_platform,param_platform);
    //IO = atoi(IO_num);
    sampling = atoi(sampling_num);
    reset_val = atoi(reset_num);

    while(1){
        printf("counter 1 task \r\n");
        // get counter value from payload
        counter_tmp1 = pulse_ch1;
        sdcard_read_file(BACKUPS_FOLDER,"cnt1",cnt_backup_val);
        counter_tmp2 = atoi(cnt_backup_val);
        counter_value = counter_tmp1+counter_tmp2;
        // reset if exceed limit
        if (counter_value > (reset_val-1)){
            counter_value = 0;
        }
        sprintf(cnt_backup_val,"%i",counter_value);
        sdcard_write_file(BACKUPS_FOLDER,"cnt1",cnt_backup_val);
        // Pack to platform payload
        timestamp = ntp_to_epoch()/1000;
        sprintf(buffer,"{\"ts\":%llu,\"sensor\":%s,\"param\":%s,\"value\":%i}",timestamp, sensor_platform, param_platform, counter_value);
        printf("buffer payload: %s\r\n",buffer);
        // save to sdcard
        sprintf(backup_name,"%i",(uint16_t)(timestamp/1000));
        printf("filename: %s\r\n",backup_name);
        sdcard_write_file(COUNTER1_BACKUP_FOLDER,backup_name,buffer);
        // check data in backup folder
        filenum = sdcard_countfile(COUNTER1_BACKUP_FOLDER);
            //printf("amount of file: %i\r\n",filenum);
        if (filenum>0){
            for (int i=0;i<filenum;i++){
                //publish 
                release_procedure(COUNTER1_BACKUP_FOLDER);
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

void pulse2_counter_task(void *arg)
{
    char* msg;
    char IO_num[5];
    char sampling_num[5];
    char reset_num[5];
    char sensor_platform[32];
    char param_platform[32];
    char buffer[512];
    int64_t timestamp = 0;
    char backup_name[8];
    int filenum = 0;
    //int IO;
    char cnt_backup_val[16];
    int sampling;
    int reset_val;
    int counter_tmp1;
    int counter_tmp2;
    int counter_value;

    msg = (char*) arg;
    printf("task msg: %s\r\n",msg);
    sscanf(msg,"%[^:]:%[^:]:%[^:]:%[^:]:%s", IO_num,reset_num,sampling_num,sensor_platform,param_platform);
    printf("IO:%s, reset_val:%s, sampling:%s, sensor:%s, param:%s\r\n", IO_num,reset_num,sampling_num,sensor_platform,param_platform);
    //IO = atoi(IO_num);
    sampling = atoi(sampling_num);
    reset_val = atoi(reset_num);

    while(1){
        printf("counter 2 task \r\n");
        // get counter value from payload
        counter_tmp1 = pulse_ch2;
        sdcard_read_file(BACKUPS_FOLDER,"cnt2",cnt_backup_val);
        counter_tmp2 = atoi(cnt_backup_val);
        counter_value = counter_tmp1+counter_tmp2;
        // reset if exceed limit
        if (counter_value > (reset_val-1)){
            counter_value = 0;
        }
        sprintf(cnt_backup_val,"%i",counter_value);
        sdcard_write_file(BACKUPS_FOLDER,"cnt2",cnt_backup_val);
        // Pack to platform payload
        timestamp = ntp_to_epoch()/1000;
        sprintf(buffer,"{\"ts\":%llu,\"sensor\":%s,\"param\":%s,\"value\":%i}",timestamp, sensor_platform, param_platform, counter_value);
        printf("buffer payload: %s\r\n",buffer);
        // save to sdcard
        sprintf(backup_name,"%i",(uint16_t)(timestamp/1000));
        printf("filename: %s\r\n",backup_name);
        sdcard_write_file(COUNTER2_BACKUP_FOLDER,backup_name,buffer);
        // check data in backup folder
        filenum = sdcard_countfile(COUNTER2_BACKUP_FOLDER);
            //printf("amount of file: %i\r\n",filenum);
        if (filenum>0){
            for (int i=0;i<filenum;i++){
                //publish 
                release_procedure(COUNTER2_BACKUP_FOLDER);
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

void pulse3_counter_task(void *arg)
{
    char* msg;
    char IO_num[5];
    char sampling_num[5];
    char reset_num[5];
    char sensor_platform[32];
    char param_platform[32];
    char buffer[512];
    int64_t timestamp = 0;
    char backup_name[8];
    int filenum = 0;
    //int IO;
    char cnt_backup_val[16];
    int sampling;
    int reset_val;
    int counter_tmp1;
    int counter_tmp2;
    int counter_value;

    msg = (char*) arg;
    printf("task msg: %s\r\n",msg);
    sscanf(msg,"%[^:]:%[^:]:%[^:]:%[^:]:%s", IO_num,reset_num,sampling_num,sensor_platform,param_platform);
    printf("IO:%s, reset_val:%s, sampling:%s, sensor:%s, param:%s\r\n", IO_num,reset_num,sampling_num,sensor_platform,param_platform);
    //IO = atoi(IO_num);
    sampling = atoi(sampling_num);
    reset_val = atoi(reset_num);

    while(1){
        printf("counter 1 task \r\n");
        // get counter value from payload
        counter_tmp1 = pulse_ch3;
        sdcard_read_file(BACKUPS_FOLDER,"cnt3",cnt_backup_val);
        counter_tmp2 = atoi(cnt_backup_val);
        counter_value = counter_tmp1+counter_tmp2;
        // reset if exceed limit
        if (counter_value > (reset_val-1)){
            counter_value = 0;
        }
        sprintf(cnt_backup_val,"%i",counter_value);
        sdcard_write_file(BACKUPS_FOLDER,"cnt3",cnt_backup_val);
        // Pack to platform payload
        timestamp = ntp_to_epoch()/1000;
        sprintf(buffer,"{\"ts\":%llu,\"sensor\":%s,\"param\":%s,\"value\":%i}",timestamp, sensor_platform, param_platform, counter_value);
        printf("buffer payload: %s\r\n",buffer);
        // save to sdcard
        sprintf(backup_name,"%i",(uint16_t)(timestamp/1000));
        printf("filename: %s\r\n",backup_name);
        sdcard_write_file(COUNTER3_BACKUP_FOLDER,backup_name,buffer);
        // check data in backup folder
        filenum = sdcard_countfile(COUNTER3_BACKUP_FOLDER);
            //printf("amount of file: %i\r\n",filenum);
        if (filenum>0){
            for (int i=0;i<filenum;i++){
                //publish 
                release_procedure(COUNTER3_BACKUP_FOLDER);
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


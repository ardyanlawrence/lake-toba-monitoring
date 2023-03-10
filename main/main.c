/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_controller.h"

void app_main(void)
{
    xTaskCreate(button_led_fsm,"button and led control",2048,NULL,1,NULL);
	xTaskCreate(main_fsm,"main finite state machine",3*4096,NULL,2,NULL);
	vTaskDelay(pdMS_TO_TICKS(1000));
	// main loop
    while(1)
    {
    	vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

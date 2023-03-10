/*
 *  gpio_app.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

#include "gpio_app.h"

QueueHandle_t io_evqueue;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(io_evqueue, &gpio_num, NULL);
}

// task for interrupt handler
static void gpio_task_handler(void* arg)
{
    uint32_t io_num;
    while(1)
    {
        if(xQueueReceive(io_evqueue, &io_num, portMAX_DELAY)) {
        	gpio_do_counting(io_num);
        }
		vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void gpio_initialize(gpio_num_t pin_num, gpio_mode_t mode, gpio_int_type_t int_type, gpio_pulldown_t pulldown_en, gpio_pullup_t pullup_en)
{
	gpio_config_t gpio_conf;

	gpio_conf.pin_bit_mask = 1ULL << pin_num;
	gpio_conf.mode = mode;
	gpio_conf.intr_type = int_type;
	gpio_conf.pull_down_en = pulldown_en;
	gpio_conf.pull_up_en = pullup_en;

	gpio_config(&gpio_conf);

	static bool install_stat = 1;
	if (int_type != GPIO_INTR_DISABLE){
		if (install_stat != 0){
			install_stat = gpio_install_isr_service(0);
			printf("gpio ret isr service = %i\r\n", install_stat);
		}
		// gpio_set_interrupt(pin_num);
		// create handler event
		//create a queue to handle gpio event from isr
		// io_evqueue = xQueueCreate(30, sizeof(uint32_t));
		// // create task for handler
		// xTaskCreate(gpio_task_handler, "gpio_task_handler", 2048, NULL, GPIO_HANDLER_PRIORITY, NULL);
	}
}

void gpio_set_interrupt(uint32_t pin_num)
{
	// if (install_stat != 0){
	// 	install_stat = gpio_install_isr_service(0);
	// 	printf("gpio ret isr service = %i\r\n", install_stat);
	// }
	// gpio_isr_handler_add(pin_num, gpio_isr_handler, (void*)pin_num);
}


void gpio_do_counting (uint32_t io_num)
{
	
	switch(io_num)
	{
		case DIGITAL_CH1: pulse_ch1++;
			break;
		case DIGITAL_CH2: pulse_ch2++;
			break;
		case DIGITAL_CH3: pulse_ch3++;
			break;
		default:
			break;
	}
}

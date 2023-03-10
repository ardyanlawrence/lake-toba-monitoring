#include "rgb.h"

void init_rgb()
{
	gpio_set_direction(BLUE_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(RED_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(GREEN_PIN, GPIO_MODE_OUTPUT);
}

void turn_on_rgb(int color)
{
    bool blue_state = 0;
    bool red_state = 0;
    bool green_state = 0;
	switch (color)
	{
	case RGB_RED:
		red_state = 1; 
		break;
	case RGB_BLUE:
		blue_state = 1; 
		break;
	case RGB_GREEN:
		green_state = 1; 
		break;
	case RGB_MAGENTA:
		blue_state = 1;
		red_state = 1;
		break;
	case RGB_YELLOW:
		red_state = 1;
		green_state = 1; 
		break;
	case RGB_CYAN:
		blue_state = 1;
		green_state = 1; 
		break;
	case RGB_WHITE:
		blue_state = 1;
		green_state = 1; 
		red_state = 1;
		break;
	default:
		break;
	}
    gpio_set_level(BLUE_PIN, blue_state);
    gpio_set_level(RED_PIN, red_state);
    gpio_set_level(GREEN_PIN, green_state);
}
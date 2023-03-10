#ifndef __RGB_H
#define __RGB_H

#include "driver/gpio.h"

#define BLUE_PIN 25
#define RED_PIN 26
#define GREEN_PIN 27

#define RGB_OFF 0
#define RGB_BLUE 1
#define RGB_RED 2
#define RGB_GREEN 3
#define RGB_MAGENTA 4
#define RGB_YELLOW 5
#define RGB_CYAN 6
#define RGB_WHITE 7

void turn_on_rgb(int color);
void init_rgb();

#endif
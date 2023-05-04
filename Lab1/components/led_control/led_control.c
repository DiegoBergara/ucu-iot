#include <stdio.h>
#include <string.h>

#include "led_control.h"
#include "led_strip.h"

led_strip_t *embedded_led;

void led_init()
{
    led_rgb_init(&embedded_led);
}

void set_color(char color[]){

   if (strcmp(color, "RED") == 0) {
        embedded_led->set_pixel(embedded_led,0,255,0,0);
        embedded_led->refresh(embedded_led,0);

    } else if (strcmp(color, "GREEN") == 0) {
        embedded_led->set_pixel(embedded_led,0,0,128,0);
        embedded_led->refresh(embedded_led,0);

    } else if (strcmp(color, "BLUE") == 0) {
        embedded_led->set_pixel(embedded_led,0,255,0,255);
        embedded_led->refresh(embedded_led,0);
    }
}
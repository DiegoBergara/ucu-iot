#include <stdio.h>
#include <string.h>

#include "led_control.h"
#include "led_strip.h"

led_strip_t *embedded_led;

void led_init()
{
    led_rgb_init(&embedded_led);
}

void set_color(uint32_t red, uint32_t green, uint32_t blue){
        embedded_led->set_pixel(embedded_led, 0, red, green, blue);
        embedded_led->refresh(embedded_led,0);
}
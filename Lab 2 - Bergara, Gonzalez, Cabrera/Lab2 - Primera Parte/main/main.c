#include <stdio.h>
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "led_control.h" // libreria creada para controlar led_strip
#include "touch_control.h"


static void read_task(void *pvParameter)
{
    int red = 0;
    int green = 0;
    int blue = 0;

    while(1){
        if(play_pause_button_pressed()){
            blue += 1;
            if(blue > 255){
                blue=0;
            }
        }

        if(vol_up_button_pressed()){
            red += 1;
            if(red > 255){
                red=0;
            }
        }

        if(vol_down_button_pressed()){
            green += 1;
            if(green > 255){
                green=0;
            }
        }
        if(record_button_pressed()){
            green = 0;
            blue = 0;
            red = 0;
        }

        set_color(red,green,blue);
    }
 }


void app_main(void)
{
    led_init();
    touch_init();
    /* Start task to read values by pads. */
    xTaskCreate(&read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
}
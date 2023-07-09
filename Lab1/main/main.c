#include <stdio.h>

#include "led_delay.h"
#include "led_control.h"

void app_main(void)
{
    led_init();

    while (1)
    {
        set_color("RED");
        delay_s(10);
        set_color("GREEN");
        delay_s(10);
        set_color("BLUE");
        delay_s(10);
    }
    
}

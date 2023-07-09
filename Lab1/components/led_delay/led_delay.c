#include <stdio.h>

#include "led_delay.h"
#include "esp_rom_sys.h"

void delay_s(uint64_t int_s) {
    esp_rom_delay_us ((uint64_t) int_s * 1000000);
}
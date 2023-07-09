#include <stdio.h>

#include "delay.h"
#include "esp_rom_sys.h"

void delay_s(uint32_t int_s) {
    esp_rom_delay_us ((uint32_t) int_s * 1000000);
}
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "sensor/sensor.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_spiffs.h"

#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_adc_cal.h"




static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Hola Mundo");
    init_sensor();
    get_sensor_value();
}

//Sensor y sntp
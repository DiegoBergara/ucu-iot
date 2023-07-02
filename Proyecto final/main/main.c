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

#include "sta-AP.h"
#include "server.h"

#include "esp_sntp.h"
#include "lwip/apps/sntp.h"

#include "lwip/opt.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/dhcp.h"


static const char *TAG = "main";

void app_main(void)
{
    time_t now;
    struct tm timeinfo;
    //1 para STA o 2 para AP 
    setMode(1);
    server_init();
    setenv("TZ","UTC+3",1);
    tzset();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    time(&now);
    localtime_r(&now, &timeinfo);
    
    // Print the current time (optional)
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "Current time: %s", strftime_buf);


    init_sensor();
    get_sensor_value();
}

//Sensor y sntp
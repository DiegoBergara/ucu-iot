#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
//#include "esp_spiffs.h"

#include "esp_system.h"
#include "driver/i2c.h"
#include "esp_err.h"
//#include "esp_adc_cal.h"

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

#include "delay.h"

time_t now;
struct tm timeinfo;

void sntpSyncTask(void* pvParameters){
    while (1) {
        if(sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET){
            delay_s(5);
        } else {
            time(&now);
            localtime_r(&now, &timeinfo);
            delay_s(10);
        }

    }
}

void initSntp(){
    setenv("TZ","UTC+3",1);
    tzset();
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    xTaskCreate(sntpSyncTask, "SNTP_SYNC", 1024, NULL, 1, NULL);
}

char* getCurrentTime(){
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    return strftime_buf;
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "sensor.h"
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


#define BUFFER_SIZE 100
#define NVS_NAMESPACE "storage"
#define NVS_KEY_LAST_VALUE "last_value"

typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint8_t sent;
    uint8_t count;
    uint8_t lastAdded;
} CircularBuffer;

void bufferInit(CircularBuffer *cb) {
    cb->sent = NULL;
    cb->count = 0;
    cb->lastAdded = NULL;
}

void bufferWrite(CircularBuffer *cb, uint8_t data) {
    *(cb->head) = data;
    cb->lastAdded = cb->head;
    cb->head++;
    cb->count++;

    if (cb->head == cb->buffer + BUFFER_SIZE) {
        cb->head = cb->buffer;
    }

    if (cb->count > BUFFER_SIZE) {
        cb->tail++;
        cb->count--;
        if (cb->tail == cb->buffer + BUFFER_SIZE) {
            cb->tail = cb->buffer;
        }
    }
}

uint8_t bufferRead(CircularBuffer *cb) {
    uint8_t data = *(cb->tail);
    cb->tail++;
    cb->count--;

    if (cb->tail == cb->buffer + BUFFER_SIZE) {
        cb->tail = cb->buffer;
    }

    return data;
}

void saveLastValue(CircularBuffer *cb) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        ESP_LOGI(__FUNCTION__, "Error al borrar el espacio de almacenamiento NVS: %d", err);
        return;
    }

    err = nvs_set_u8(nvsHandle, NVS_KEY_LAST_VALUE, cb->lastAdded);
    if (err != ESP_OK) {
        ESP_LOGI(__FUNCTION__, "Error al guardar el último valor agregado en NVS: %d", err);
    }
    nvs_commit(nvsHandle);
    nvs_close(nvsHandle);
}

void loadLastValue(CircularBuffer *cb) {
    nvs_handle_t nvsHandle;
    esp_err_t err;

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        ESP_LOGI(__FUNCTION__, "Error al abrir el espacio de almacenamiento NVS: %d", err);
        return;
    }

    uint8_t lastValue;
    err = nvs_get_u8(nvsHandle, NVS_KEY_LAST_VALUE, &lastValue);
    if (err == ESP_OK) {
        cb->lastAdded = lastValue;
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGI(__FUNCTION__, "No se encontró el último valor agregado en NVS");
    } else {
        ESP_LOGI(__FUNCTION__, "Error al leer el último valor agregado desde NVS");
    }

    nvs_close(nvsHandle);
}


static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Hola Mundo");
    // init_sensor();
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

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    CircularBuffer cb;
    bufferInit(&cb);

    // Cargar el último valor agregado desde NVS
    loadLastValue(&cb);

    // Escribir datos de prueba en el buffer
    for (int i = 0; i < BUFFER_SIZE + 5; i++) {
        bufferWrite(&cb, i);
    }

    // Guardar el último valor agregado en NVS
    saveLastValue(&cb);

    // Leer y mostrar los datos del buffer
    while (cb.count > 0) {
        ESP_LOGI(__FUNCTION__, "%u", bufferRead(&cb));
    }
}
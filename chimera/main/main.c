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


#define BUFFER_SIZE 100
#define NVS_NAMESPACE "buffer_namespace"
#define NVS_KEY_LAST_VALUE "last_value"

typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint8_t *head;
    uint8_t *tail;
    size_t count;
    uint8_t *lastAdded;
} CircularBuffer;

void bufferInit(CircularBuffer *cb) {
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
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
    esp_err_t err = nvs_flash_erase();
    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        printf("Error al borrar el espacio de almacenamiento NVS: %d\n", err);
        return;
    }

    err = nvs_set_u8(nvsHandle, NVS_KEY_LAST_VALUE, *(cb->lastAdded));
    if (err != ESP_OK) {
        printf("Error al guardar el último valor agregado en NVS: %d\n", err);
    }

    nvs_close(nvsHandle);
}

void loadLastValue(CircularBuffer *cb) {
    nvs_handle_t nvsHandle;
    esp_err_t err;

    err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvsHandle);
    if (err != ESP_OK) {
        printf("Error al abrir el espacio de almacenamiento NVS: %d\n", err);
        return;
    }

    uint8_t lastValue;
    err = nvs_get_u8(nvsHandle, NVS_KEY_LAST_VALUE, &lastValue);
    if (err == ESP_OK) {
        *(cb->lastAdded) = lastValue;
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        printf("No se encontró el último valor agregado en NVS\n");
    } else {
        printf("Error al leer el último valor agregado desde NVS\n");
    }

    nvs_close(nvsHandle);
}


static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Hola Mundo");
    init_sensor();


    nvs_flash_init();

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
        printf("%u ", bufferRead(&cb));
    }
    printf("\n");
}

//Sensor y sntp
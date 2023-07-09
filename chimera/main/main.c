#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "nvs_flash.h"
#include "nvs.h"
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

#include "circularBuffer.h"
#include "sntpServer.h"
#include "sensor.h"
#include "delay.h"

void readSensorTask(void* pvParameters){
    while(1){
        // AGREGAR MUTEX PARA LA LECTURA
        CircularBuffer* buffer = loadBufferFromFlash();
        if (isBufferCorrupted(buffer)) {
            buffer = createBuffer();
        } else {
            char* currentTimestamp = getCurrentTime();
            uint16_t currentValue = get_sensor_value();
            CircularBufferElement* bufferElement = (CircularBufferElement*)malloc(sizeof(CircularBufferElement));
            bufferElement->value = currentValue;
            bufferElement->timestamp = currentTimestamp;
            insertElement(buffer, *bufferElement);
            saveBufferToFlash(buffer);
            delay_s(5);
        }
    }
}

void sendValueTask(void* pvParameters){
    while(1){
        // AGREGAR MUTEX PARA LA LECTURA
        CircularBuffer* buffer = loadBufferFromFlash();
        if (isBufferCorrupted(buffer)) {
            buffer = createBuffer();
        } else {
           if(buffer->sent != buffer->rear) { 
            CircularBufferElement elemToSend = buffer->data[buffer->rear];
            bool sendOK = false;
            // intenta enviar
            if (sendOK){
                if(buffer->sent == 99){
                    buffer->sent = 0;
                } else {
                    buffer->sent++; 
                }
            }}
        }
        saveBufferToFlash(buffer);
    }
}

void app_main() {
    // Inicia NVS
    esp_err_t nvsErr = nvs_flash_init();
    if (nvsErr == ESP_ERR_NVS_NO_FREE_PAGES || nvsErr == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvsErr = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvsErr);

    // Iniciar Server y STA-AP

    // Inicia SNTP
    initSntp();

    // Tareas
    xTaskCreate(readSensorTask, "READ_TASK", 5120, NULL, 10, NULL);
    xTaskCreate(sendValueTask, "SEND_TASK", 5120, NULL, 10, NULL);
}

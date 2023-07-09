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



void app_main() {
    // Initialize NVS
    esp_err_t nvsErr = nvs_flash_init();
    if (nvsErr == ESP_ERR_NVS_NO_FREE_PAGES || nvsErr == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvsErr = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvsErr);

    CircularBuffer* buffer = createBuffer();

    insertElement(buffer, 10);
    insertElement(buffer, 20);
    insertElement(buffer, 30);
    insertElement(buffer, 40);

    printf("Latest element: %d\n", getLatestElement(buffer));

    int removedElement = removeLatestElement(buffer);
    printf("Removed element: %d\n", removedElement);

    printf("Latest element after removal: %d\n", getLatestElement(buffer));

    saveBufferToFlash(buffer);
    CircularBuffer* loadedBuffer = loadBufferFromFlash();

    if (isBufferCorrupted(loadedBuffer)) {
        printf("Loaded buffer is corrupted.\n");
    } else {
        printf("Loaded buffer is not corrupted.\n");
    }

    destroyBuffer(buffer);
    destroyBuffer(loadedBuffer);
}

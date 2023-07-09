#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "nvs_flash.h"
#include "nvs.h"

#include "circularBuffer.h"

#define BUFFER_SIZE 100
#define NVS_NAMESPACE "storage"
#define NVS_KEY_LAST_VALUE "circular_buffer"


CircularBuffer* createBuffer() {
    CircularBuffer* buffer = (CircularBuffer*)malloc(sizeof(CircularBuffer));
    buffer->front = 0;
    buffer->rear = -1;
    buffer->count = 0;
    buffer->checksum = 0;
    return buffer;
}

void destroyBuffer(CircularBuffer* buffer) {
    free(buffer);
}

int isBufferEmpty(CircularBuffer* buffer) {
    return buffer->count == 0;
}

int isBufferFull(CircularBuffer* buffer) {
    return buffer->count == BUFFER_SIZE;
}

void updateChecksum(CircularBuffer* buffer) {
    buffer->checksum = 0;
    for (int i = buffer->front; i <= buffer->rear; i = (i + 1) % BUFFER_SIZE) {
        buffer->checksum ^= buffer->data[i];
    }
}

void insertElement(CircularBuffer* buffer, uint16_t element) {
    if (isBufferFull(buffer)) {
        printf("Error: Buffer is full, unable to insert element.\n");
        return;
    }

    buffer->rear = (buffer->rear + 1) % BUFFER_SIZE;
    buffer->data[buffer->rear] = element;
    buffer->count++;
    updateChecksum(buffer);
}

uint16_t getLatestElement(CircularBuffer* buffer) {
    if (isBufferEmpty(buffer)) {
        printf("Error: Buffer is empty, no elements to retrieve.\n");
        return 0;  // You can choose a different value to indicate an error condition
    }

    return buffer->data[buffer->rear];
}

uint16_t removeLatestElement(CircularBuffer* buffer) {
    if (isBufferEmpty(buffer)) {
        return 0;  // You can choose a different value to indicate an error condition
    }

    uint16_t removedElement = buffer->data[buffer->rear];
    buffer->rear = (buffer->rear - 1 + BUFFER_SIZE) % BUFFER_SIZE;
    buffer->count--;
    updateChecksum(buffer);

    return removedElement;
}

bool isBufferCorrupted(CircularBuffer* buffer) {
    uint16_t calculatedChecksum = 0;
    for (int i = buffer->front; i <= buffer->rear; i = (i + 1) % BUFFER_SIZE) {
        calculatedChecksum ^= buffer->data[i];
    }

    return calculatedChecksum != buffer->checksum;
}

void saveBufferToFlash(CircularBuffer* buffer) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        printf("Failed to open NVS handle\n");
        return;
    }

    err = nvs_set_blob(nvsHandle, "circular_buffer", buffer, sizeof(CircularBuffer));
    if (err != ESP_OK) {
        printf("Failed to set circular buffer in NVS\n");
    }

    err = nvs_commit(nvsHandle);
    if (err != ESP_OK) {
        printf("Failed to commit NVS changes\n");
    }

    nvs_close(nvsHandle);
}

CircularBuffer* loadBufferFromFlash() {
    CircularBuffer* buffer = createBuffer();

    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvsHandle);
    if (err != ESP_OK) {
        printf("Failed to open NVS handle\n");
        return buffer;
    }

    size_t bufferSize = sizeof(CircularBuffer);
    err = nvs_get_blob(nvsHandle, "circular_buffer", buffer, &bufferSize);
    if (err != ESP_OK) {
        printf("Failed to get circular buffer from NVS\n");
    }

    nvs_close(nvsHandle);

    return buffer;
}

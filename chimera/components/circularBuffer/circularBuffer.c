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
    buffer->rear = 0;
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
        buffer->checksum ^= buffer->data[i].value;
    }
}

void insertElement(CircularBuffer* buffer, CircularBufferElement element) {
    if (isBufferFull(buffer)) {
        free(&(buffer->data[0]));
        buffer->count = 0;
        buffer->rear = 0;
    }

    buffer->data[buffer->rear] = element;
    buffer->rear = (buffer->rear + 1) % BUFFER_SIZE; // CHECKEAR ESTE % BUFFER
    buffer->count++;
    updateChecksum(buffer);
}

bool isBufferCorrupted(CircularBuffer* buffer) {
    uint16_t calculatedChecksum = 0;
    for (int i = buffer->front; i <= buffer->rear; i = (i + 1) % BUFFER_SIZE) {
        calculatedChecksum ^= buffer->data[i].value;
    }

    return calculatedChecksum != buffer->checksum;
}

void saveBufferToFlash(CircularBuffer* buffer) {
    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsHandle);
    if (err != ESP_OK) {
        return;
    }

    err = nvs_set_blob(nvsHandle, "circular_buffer", buffer, sizeof(CircularBuffer));

    err = nvs_commit(nvsHandle);

    nvs_close(nvsHandle);
}

CircularBuffer* loadBufferFromFlash() {
    CircularBuffer* buffer = createBuffer();

    nvs_handle_t nvsHandle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvsHandle);
    if (err != ESP_OK) {
        return buffer;
    }

    size_t bufferSize = sizeof(CircularBuffer);
    err = nvs_get_blob(nvsHandle, "circular_buffer", buffer, &bufferSize);

    nvs_close(nvsHandle);

    return buffer;
}

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h" 
#include "driver/uart.h"

#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "led_control.h"
#include "led_strip.h"

#define LED_RED '0'
#define LED_GREEN '1'
#define LED_BLUE '2'

#define ECHO_TEST_TXD (GPIO_NUM_42)
#define ECHO_TEST_RXD (GPIO_NUM_44)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (0)
#define ECHO_UART_BAUD_RATE     (115200)
#define ECHO_TASK_STACK_SIZE    (5120)

static const char *TAG = "UART TEST";

#define BUF_SIZE (1024)

QueueHandle_t queue;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;


void timer_callback(xTimerHandle xTimer) {
    RGB_t *color = (RGB_t*)pvTimerGetTimerID(xTimer);
    xQueueSend(queue, color, portMAX_DELAY);
    ESP_LOGI(TAG, "Enviar color luego de: %hhu, %hhu, %hhu", color->r, color->g, color->b);
    free(color); 
}

void TaskC(void* pvParameters) {
    RGB_t color;
    while(1) {
        ESP_LOGI("LEEED", "Recv RGB: %hhu, %hhu, %hhu", color.r, color.g, color.b);
        if (xQueueReceive(queue, &color, portMAX_DELAY)) {
            set_color(color.r, color.g, color.b);
        }
    }
}


static void TaskB(void *arg)
{
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_RATE_MS);
        data[len] = '\0';
        RGB_t color;

        if (len) {
            uint32_t delay;
            RGB_t *color = malloc(sizeof(RGB_t));
            if(sscanf((char*)data, "%hhu,%hhu,%hhu,%u", &(color->r), &(color->g), &(color->b), &delay) == 4) {
                if (delay == 0) {
                    // Si el delay es cero, enviar inmediatamente
                    xQueueSend(queue, color, portMAX_DELAY);
                    ESP_LOGI(TAG, "Recv RGB: %hhu, %hhu, %hhu", color->r, color->g, color->b);
                    free(color);
                 } else {
                    // Si el delay es distinto de cero, usar timer
                    xTimerHandle timer = xTimerCreate("ColorTimer", pdMS_TO_TICKS(delay), pdFALSE, color, timer_callback);
                    xTimerStart(timer, 0);
                     ESP_LOGI(TAG, "El tiempo comienza con delay: %u", delay);
                }
            } else {
                ESP_LOGE(TAG, "Error al leer rgb y timer");
                free(color);
            }
        }
    }
}

void app_main() {
    queue = xQueueCreate(10, sizeof(RGB_t));
    led_init();
    // Conectarse al puerto uart via putty
    // Serial Line: (donde este conectada la placa)
    // Speed: 115200
    xTaskCreate(TaskB, "UART TASK", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
    xTaskCreate(TaskC, "LED Task", 2048, NULL, 1, NULL);
}
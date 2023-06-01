#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "driver/uart.h"

#define MAX_LINE_LENGTH 256

void app_main(void)
{
    // Configurar UART para la consola
    uart_config_t uart_config = {
        .baud_rate = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    // Instalar el controlador UART y configurarlo
    ESP_ERROR_CHECK(uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(CONFIG_ESP_CONSOLE_UART_NUM, &uart_config));

    // Configurar el sistema de archivos VFS
    esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
    uart_set_line_inverse(CONFIG_ESP_CONSOLE_UART_NUM, UART_SIGNAL_TXD_INV);

    // Inicializar la consola
    esp_console_config_t console_config = {
        .max_cmdline_args = 8,
        .max_cmdline_length = 256,
    };
    ESP_ERROR_CHECK(esp_console_init(&console_config));

    // Configurar el sistema de archivos FAT para la consola
    esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 4,
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    wl_handle_t s_wl_handle; // Declaración de la variable s_wl_handle
    ESP_ERROR_CHECK(esp_vfs_fat_spiflash_mount("/fat", "storage", &mount_config, &s_wl_handle));

    printf("Texto de ejemplo\n");

    while (1) {
        // Leer línea de texto desde la consola
        char line[MAX_LINE_LENGTH];
        fgets(line, sizeof(line), stdin);

        // Eliminar el carácter de nueva línea si está presente
        char *newline = strchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        // Realizar acciones con el texto recibido
        printf("Texto recibido: %s\n", line);
    }
}

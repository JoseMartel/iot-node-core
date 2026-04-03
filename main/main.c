#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "system_utils.h"
#include "config_manager.h"
#include "network_manager.h"
#include "protocol_manager.h"
#include "storage_manager.h"
#include "sensor_manager.h"
#include "web_server.h"
#include "system_types.h"

static const char *TAG = "IOT_NODE_MAIN";

static QueueHandle_t data_queue = NULL;
#define QUEUE_SIZE 20 

void sensor_task(void *pvParameters) {
    while (1) {
        iot_packet_t packet;
        if (sensor_manager_read(&packet) == ESP_OK) {
            if (xQueueSend(data_queue, &packet, 0) != pdPASS) {
                ESP_LOGW(TAG, "RAM Queue full, move to NVS.");
                storage_push_packet(&packet);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(config_get_reading_interval() * 1000));
    }
}

void sender_task(void *pvParameters) {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(config_get_sending_interval() * 1000));

        if (!network_is_connected()) {
            // Sin red: Vaciamos RAM a NVS inmediatamente para asegurar persistencia
            iot_packet_t ram_packet;
            while (xQueueReceive(data_queue, &ram_packet, 0) == pdPASS) {
                storage_push_packet(&ram_packet);
            }
            continue;
        }

        // --- PRIORIDAD 1: DATOS RECIENTES EN RAM (LIFO) ---
        iot_packet_t ram_packet;
        while (xQueueReceive(data_queue, &ram_packet, 0) == pdPASS) {
            int status = protocol_send_packet(&ram_packet);
            
            if (status == 200) {
                ESP_LOGI(TAG, "RAM Packet %lu: Success ACK.", (unsigned long)ram_packet.msg_id);
                // El dato ya fue eliminado de la cola por xQueueReceive
            } else if (status >= 400 && status < 500) {
                ESP_LOGE(TAG, "RAM Packet %lu: Malformed (HTTP %d). Dropping.", (unsigned long)ram_packet.msg_id, status);
            } else {
                // Error de conexión o 5xx del servidor: Mover a NVS para persistencia
                ESP_LOGW(TAG, "RAM Packet %lu: Failed (HTTP %d). Moving to NVS Storage.", (unsigned long)ram_packet.msg_id, status);
                storage_push_packet(&ram_packet);
                // Si hubo error de conexión, es probable que los siguientes también fallen
                if (status < 0) break; 
            }
        }

        // --- PRIORIDAD 2: DATOS HISTÓRICOS EN NVS (FIFO) ---
        while (storage_get_pending_count() > 0) {
            iot_packet_t nvs_packet;
            // Solo hacemos 'pop' si estamos seguros de que fue enviado
            // storage_pop_packet elimina internamente, para esta lógica 
            // idealmente necesitaríamos un 'peek' o un 'confirm_pop'
            // Por ahora usamos la lógica de pop y re-guardar o confiar en el estado
            
            if (storage_pop_packet(&nvs_packet) == ESP_OK) {
                int status = protocol_send_packet(&nvs_packet);
                
                if (status == 200) {
                    ESP_LOGI(TAG, "NVS Packet %lu: Success ACK. Removed.", (unsigned long)nvs_packet.msg_id);
                } else if (status >= 400 && status < 500) {
                    ESP_LOGE(TAG, "NVS Packet %lu: Malformed. Dropping from Flash.", (unsigned long)nvs_packet.msg_id);
                } else {
                    // Falló la red de nuevo: Re-guardar en NVS y detener vaciado
                    ESP_LOGW(TAG, "NVS Packet %lu: Resend Failed. Re-storing.", (unsigned long)nvs_packet.msg_id);
                    storage_push_packet(&nvs_packet);
                    break;
                }
            }
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting IoT Node Core (Inverse Priority + ACK logic)...");

    ESP_ERROR_CHECK(system_utils_init());
    ESP_ERROR_CHECK(config_manager_init());
    ESP_ERROR_CHECK(storage_manager_init());
    ESP_ERROR_CHECK(sensor_manager_init());
    ESP_ERROR_CHECK(network_manager_init());
    ESP_ERROR_CHECK(web_server_init());

    data_queue = xQueueCreate(QUEUE_SIZE, sizeof(iot_packet_t));
    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
    xTaskCreate(sender_task, "sender_task", 4096, NULL, 4, NULL);
}

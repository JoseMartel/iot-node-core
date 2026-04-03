#include "sensor_manager.h"
#include "esp_log.h"
#include "esp_system.h"
#include <stdlib.h>
#include <time.h>

static const char *TAG = "SENSOR_MANAGER";
static uint32_t current_msg_id = 0;

esp_err_t sensor_manager_init(void) {
    ESP_LOGI(TAG, "Simulated Sensors Init...");
    return ESP_OK;
}

esp_err_t sensor_manager_read(iot_packet_t* packet) {
    packet->msg_id = current_msg_id++;
    packet->timestamp = (uint32_t)time(NULL);
    packet->temperature = 22.5 + (rand() % 50) / 10.0;
    packet->humidity = 50.0 + (rand() % 300) / 10.0;
    packet->battery_mv = 3700 + (rand() % 500);
    packet->free_heap = (uint32_t)esp_get_free_heap_size();
    packet->status_code = 0x01; // Online flag
    
    return ESP_OK;
}

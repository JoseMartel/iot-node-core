#include "system_utils.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "SYSTEM_UTILS";

esp_err_t system_utils_init(void) {
    ESP_LOGI(TAG, "Initializing System Utilities...");
    return ESP_OK;
}

void system_utils_log_health(void) {
    uint32_t free_heap = esp_get_free_heap_size();
    uint32_t min_free_heap = esp_get_minimum_free_heap_size();
    
    ESP_LOGI(TAG, "[HEALTH] Free Heap: %lu bytes (Min: %lu)", (unsigned long)free_heap, (unsigned long)min_free_heap);
    ESP_LOGI(TAG, "[HEALTH] Running Tasks: %d", (int)uxTaskGetNumberOfTasks());
}

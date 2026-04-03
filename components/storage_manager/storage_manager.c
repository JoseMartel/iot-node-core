#include "storage_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "STORAGE_MANAGER";
static const char *NAMESPACE = "off_data";
static const char *KEY_HEAD = "head";
static const char *KEY_TAIL = "tail";
static const uint16_t MAX_CAPACITY = 1024; // Capacidad aumentada a 1024

static uint16_t current_head = 0;
static uint16_t current_tail = 0;
static uint16_t pending_count = 0;

esp_err_t storage_manager_init(void) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        if (nvs_get_u16(handle, KEY_HEAD, &current_head) != ESP_OK) current_head = 0;
        if (nvs_get_u16(handle, KEY_TAIL, &current_tail) != ESP_OK) current_tail = 0;
        
        // Calcular pendientes de forma circular
        if (current_head >= current_tail) {
            pending_count = current_head - current_tail;
        } else {
            pending_count = (MAX_CAPACITY - current_tail) + current_head;
        }
        ESP_LOGI(TAG, "Storage LIFO Initialized. Pending: %d, Head: %d, Tail: %d", 
                 pending_count, current_head, current_tail);
        nvs_close(handle);
    }
    return err;
}

static esp_err_t update_indices(nvs_handle_t handle) {
    esp_err_t err = nvs_set_u16(handle, KEY_HEAD, current_head);
    if (err == ESP_OK) err = nvs_set_u16(handle, KEY_TAIL, current_tail);
    if (err == ESP_OK) err = nvs_commit(handle);
    return err;
}

esp_err_t storage_push_packet(const iot_packet_t* packet) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    char key[12];
    snprintf(key, sizeof(key), "d%d", current_head);
    
    err = nvs_set_blob(handle, key, packet, sizeof(iot_packet_t));
    if (err == ESP_OK) {
        current_head = (current_head + 1) % MAX_CAPACITY;
        
        // CIRCULAR: Si head alcanza a tail, sobreescribimos el más antiguo moviendo tail
        if (current_head == current_tail) {
            current_tail = (current_tail + 1) % MAX_CAPACITY;
            ESP_LOGW(TAG, "Buffer full! Overwriting oldest record. Tail moved to %d", current_tail);
        } else {
            if (pending_count < MAX_CAPACITY) pending_count++;
        }
        
        err = update_indices(handle);
    }
    
    nvs_close(handle);
    return err;
}

/**
 * @brief Implementación LIFO (Last-In, First-Out)
 * Extrae el dato apuntado por (head - 1)
 */
esp_err_t storage_pop_packet(iot_packet_t* packet) {
    if (current_head == current_tail) return ESP_ERR_NOT_FOUND;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return err;

    // LIFO: Retroceder head para obtener el último insertado
    uint16_t last_index = (current_head - 1 + MAX_CAPACITY) % MAX_CAPACITY;
    
    char key[12];
    snprintf(key, sizeof(key), "d%d", last_index);
    
    size_t size = sizeof(iot_packet_t);
    err = nvs_get_blob(handle, key, packet, &size);
    if (err == ESP_OK) {
        current_head = last_index; // El nuevo head es el índice del que acabamos de sacar
        pending_count--;
        err = update_indices(handle);
        ESP_LOGI(TAG, "LIFO Pop from %s. Remaining: %d", key, pending_count);
    }
    
    nvs_close(handle);
    return err;
}

uint16_t storage_get_pending_count(void) {
    return pending_count;
}

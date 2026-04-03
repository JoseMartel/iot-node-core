#include "config_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "CONFIG_MANAGER";
static const char *NVS_NAMESPACE = "storage";
static const char *KEY_READ_INT = "read_int";
static const char *KEY_SEND_INT = "send_int";

#define DEFAULT_READ_INT 10   // 10 seconds
#define DEFAULT_SEND_INT 60   // 60 seconds

static uint32_t current_read_int = DEFAULT_READ_INT;
static uint32_t current_send_int = DEFAULT_SEND_INT;

esp_err_t config_manager_init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    nvs_handle_t handle;
    if (nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle) == ESP_OK) {
        if (nvs_get_u32(handle, KEY_READ_INT, &current_read_int) != ESP_OK) current_read_int = DEFAULT_READ_INT;
        if (nvs_get_u32(handle, KEY_SEND_INT, &current_send_int) != ESP_OK) current_send_int = DEFAULT_SEND_INT;
        nvs_close(handle);
    }
    ESP_LOGI(TAG, "Config Loaded: Read=%lu s, Send=%lu s", 
             (unsigned long)current_read_int, (unsigned long)current_send_int);
    return ESP_OK;
}

uint32_t config_get_reading_interval(void) { return current_read_int; }
uint32_t config_get_sending_interval(void) { return current_send_int; }

esp_err_t config_set_reading_interval(uint32_t interval_s) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        err = nvs_set_u32(handle, KEY_READ_INT, interval_s);
        if (err == ESP_OK) {
            nvs_commit(handle);
            current_read_int = interval_s;
        }
        nvs_close(handle);
    }
    return err;
}

esp_err_t config_set_sending_interval(uint32_t interval_s) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        err = nvs_set_u32(handle, KEY_SEND_INT, interval_s);
        if (err == ESP_OK) {
            nvs_commit(handle);
            current_send_int = interval_s;
        }
        nvs_close(handle);
    }
    return err;
}

#include "network_manager.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "config_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include <string.h>

static const char *TAG = "NETWORK_MANAGER";
static bool is_connected = false;
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        is_connected = false;
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        is_connected = true;
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }
}

esp_err_t network_manager_init(void) {
    ESP_LOGI(TAG, "Initializing Network Manager (APSTA Robust Mode)...");
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    // 1. Configuración del Access Point
    wifi_config_t wifi_config_ap = {
        .ap = {
            .ssid = "IoT-Node-Config",
            .ssid_len = strlen("IoT-Node-Config"),
            .channel = 1,
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        },
    };

    // 2. Intentar cargar credenciales de Estación
    char ssid[32], pass[64];
    config_get_wifi_ssid(ssid, sizeof(ssid));
    config_get_wifi_pass(pass, sizeof(pass));

    if (strlen(ssid) > 0) {
        ESP_LOGI(TAG, "Configuring STA for SSID: %s", ssid);
        wifi_config_t wifi_config_sta = {0};
        strncpy((char*)wifi_config_sta.sta.ssid, ssid, sizeof(wifi_config_sta.sta.ssid));
        strncpy((char*)wifi_config_sta.sta.password, pass, sizeof(wifi_config_sta.sta.password));

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA)); 
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config_sta));
    } else {
        ESP_LOGW(TAG, "No credentials found. Starting in AP mode.");
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    }

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap));
    ESP_ERROR_CHECK(esp_wifi_start());

    if (strlen(ssid) > 0) {
        ESP_LOGI(TAG, "Waiting 10s for WiFi connection...");
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, pdFALSE, pdFALSE, pdMS_TO_TICKS(10000));
    }

    return ESP_OK;
}

bool network_is_connected(void) { return is_connected; }

esp_err_t network_connect(const char* ssid, const char* password) {
    config_set_wifi_credentials(ssid, password);
    esp_restart();
    return ESP_OK;
}

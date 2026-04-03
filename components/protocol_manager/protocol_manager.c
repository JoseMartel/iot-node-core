#include "protocol_manager.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "system_config.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "PROTOCOL_MANAGER";

int protocol_send_packet(const iot_packet_t* packet) {
    char full_url[128];
    snprintf(full_url, sizeof(full_url), "%s%s", CLOUD_BASE_URL, ENDPOINT_DATA);

    char json_payload[256];
    snprintf(json_payload, sizeof(json_payload), 
             "{\"id\":%lu,\"ts\":%lu,\"tmp\":%.2f,\"hum\":%.2f,\"bat\":%u,\"hp\":%lu,\"st\":%u}",
             (unsigned long)packet->msg_id, (unsigned long)packet->timestamp,
             packet->temperature, packet->humidity, packet->battery_mv,
             (unsigned long)packet->free_heap, packet->status_code);

    esp_http_client_config_t config = {
        .url = full_url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = HTTP_TIMEOUT_MS,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json_payload, strlen(json_payload));

    esp_err_t err = esp_http_client_perform(client);
    int status_code = -1;

    if (err == ESP_OK) {
        status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP Response: %d", status_code);
    } else {
        ESP_LOGE(TAG, "HTTP connection error: %s", esp_err_to_name(err));
        status_code = -1; // Connection error
    }

    esp_http_client_cleanup(client);
    return status_code;
}

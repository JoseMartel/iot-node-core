#include "dns_server.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "DNS_SERVER";
static int dns_socket = -1;
static TaskHandle_t dns_task_handle = NULL;

#define DNS_PORT 53
#define DNS_IP "192.168.4.1"

static void dns_server_task(void *pvParameters) {
    uint8_t data[512];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    dns_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (dns_socket < 0) {
        ESP_LOGE(TAG, "Socket creation failed");
        vTaskDelete(NULL);
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DNS_PORT);

    if (bind(dns_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Socket bind failed");
        close(dns_socket);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "DNS Server running on port %d...", DNS_PORT);

    while (1) {
        int len = recvfrom(dns_socket, data, sizeof(data), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (len > 0) {
            // DNS Reply Header: QR=1 (Response), AA=1, Code=0 (Success)
            data[2] |= 0x80; // QR = 1
            data[3] = 0x80; // RA = 1, Code = 0

            // Answers Count = 1
            data[6] = 0; data[7] = 1;

            // Append the Answer (Simplified): Always 192.168.4.1
            int pos = len;
            data[pos++] = 0xc0; data[pos++] = 0x0c; // Pointer to name
            data[pos++] = 0x00; data[pos++] = 0x01; // Type: A
            data[pos++] = 0x00; data[pos++] = 0x01; // Class: IN
            data[pos++] = 0x00; data[pos++] = 0x00; data[pos++] = 0x00; data[pos++] = 0x3c; // TTL: 60s
            data[pos++] = 0x00; data[pos++] = 0x04; // Data Length: 4 bytes
            data[pos++] = 192; data[pos++] = 168; data[pos++] = 4; data[pos++] = 1; // IP

            sendto(dns_socket, data, pos, 0, (struct sockaddr *)&client_addr, client_addr_len);
        }
    }
}

void dns_server_start(void) {
    xTaskCreate(dns_server_task, "dns_server_task", 3072, NULL, 5, &dns_task_handle);
}

void dns_server_stop(void) {
    if (dns_socket != -1) {
        close(dns_socket);
        dns_socket = -1;
    }
    if (dns_task_handle) {
        vTaskDelete(dns_task_handle);
        dns_task_handle = NULL;
    }
}

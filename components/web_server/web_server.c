#include "web_server.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "config_manager.h"
#include "storage_manager.h"
#include "system_utils.h"
#include "cJSON.h"
#include <string.h>

static const char *TAG = "WEB_SERVER";
static httpd_handle_t server = NULL;

/* Símbolos del archivo embebido */
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");

/* GET /: Servir el dashboard moderno */
static esp_err_t index_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
}

/* GET /api/status: Estado en tiempo real */
static esp_err_t status_get_handler(httpd_req_t *req) {
    char json_response[256];
    snprintf(json_response, sizeof(json_response),
             "{\"heap\":%lu,\"pending\":%d,\"uptime\":%lu}",
             (unsigned long)esp_get_free_heap_size(),
             (int)storage_get_pending_count(),
             (unsigned long)esp_log_timestamp() / 1000);
    
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
}

/* GET /api/config: Leer parámetros actuales */
static esp_err_t config_get_handler(httpd_req_t *req) {
    char json_response[128];
    snprintf(json_response, sizeof(json_response),
             "{\"read_int\":%lu,\"send_int\":%lu}",
             (unsigned long)config_get_reading_interval(),
             (unsigned long)config_get_sending_interval());
    
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
}

/* POST /api/config: Guardar nuevos parámetros */
static esp_err_t config_post_handler(httpd_req_t *req) {
    char content[128];
    int ret = httpd_req_recv(req, content, sizeof(content));
    if (ret <= 0) return ESP_FAIL;
    content[ret] = '\0';

    cJSON *root = cJSON_Parse(content);
    if (root) {
        cJSON *read_int = cJSON_GetObjectItem(root, "read_int");
        cJSON *send_int = cJSON_GetObjectItem(root, "send_int");
        
        if (read_int) config_set_reading_interval(read_int->valueint);
        if (send_int) config_set_sending_interval(send_int->valueint);
        
        cJSON_Delete(root);
        ESP_LOGI(TAG, "Config updated via Web API");
        return httpd_resp_sendstr(req, "{\"status\":\"ok\"}");
    }
    return httpd_resp_send_500(req);
}

esp_err_t web_server_init(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root_uri = { .uri = "/", .method = HTTP_GET, .handler = index_get_handler };
        httpd_register_uri_handler(server, &root_uri);

        httpd_uri_t status_uri = { .uri = "/api/status", .method = HTTP_GET, .handler = status_get_handler };
        httpd_register_uri_handler(server, &status_uri);

        httpd_uri_t config_uri_get = { .uri = "/api/config", .method = HTTP_GET, .handler = config_get_handler };
        httpd_register_uri_handler(server, &config_uri_get);

        httpd_uri_t config_uri_post = { .uri = "/api/config", .method = HTTP_POST, .handler = config_post_handler };
        httpd_register_uri_handler(server, &config_uri_post);

        return ESP_OK;
    }
    return ESP_FAIL;
}

void web_server_stop(void) {
    if (server) httpd_stop(server);
}

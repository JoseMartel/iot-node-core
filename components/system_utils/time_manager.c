#include "time_manager.h"
#include "esp_sntp.h"
#include "esp_log.h"
#include <time.h>

static const char *TAG = "TIME_MANAGER";

void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void time_manager_init(void) {
    ESP_LOGI(TAG, "Initializing SNTP...");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(1, "time.google.com");
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();
}

bool time_manager_is_synced(void) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Si el año es mayor a 2023, asumimos que está sincronizado
    if (timeinfo.tm_year < (2023 - 1900)) {
        return false;
    }
    return true;
}

uint32_t time_manager_get_timestamp(void) {
    time_t now;
    time(&now);
    return (uint32_t)now;
}

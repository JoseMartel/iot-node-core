#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "esp_err.h"
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Initialize the NVS and load initial configurations.
 */
esp_err_t config_manager_init(void);

/**
 * @brief Get and Set WiFi Credentials.
 */
void config_get_wifi_ssid(char* ssid, size_t len);
void config_get_wifi_pass(char* pass, size_t len);
esp_err_t config_set_wifi_credentials(const char* ssid, const char* pass);

/**
 * @brief Get the intervals in seconds.
 */
uint32_t config_get_reading_interval(void);
uint32_t config_get_sending_interval(void);

/**
 * @brief Set and save the intervals.
 */
esp_err_t config_set_reading_interval(uint32_t interval_s);
esp_err_t config_set_sending_interval(uint32_t interval_s);

#endif // CONFIG_MANAGER_H

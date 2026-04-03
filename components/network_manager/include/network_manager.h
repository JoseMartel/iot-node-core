#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <stdbool.h>
#include "esp_err.h"

/**
 * @brief Initialize the network manager (initially for WiFi).
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t network_manager_init(void);

/**
 * @brief Check if the device is currently connected to the network.
 * 
 * @return true If connected (either via WiFi or Cellular).
 * @return false If not connected.
 */
bool network_is_connected(void);

/**
 * @brief Start a connection attempt.
 * 
 * @param ssid WiFi SSID (if applicable)
 * @param password WiFi Password (if applicable)
 * @return esp_err_t ESP_OK if the process started.
 */
esp_err_t network_connect(const char* ssid, const char* password);

#endif // NETWORK_MANAGER_H

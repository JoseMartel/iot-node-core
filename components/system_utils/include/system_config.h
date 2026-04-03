#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/**
 * @file system_config.h
 * @brief Centralized configuration for the IoT Node Core.
 */

// --- Cloud Server Configuration ---
#define CLOUD_BASE_URL      "http://tu-servidor-propio.com"
#define ENDPOINT_DATA       "/data"
#define ENDPOINT_CONFIG     "/config"

// --- Connection Timeout Configuration ---
#define HTTP_TIMEOUT_MS     5000

// --- System Identifiers ---
#define DEVICE_TYPE         "iot-node-generic"
#define FIRMWARE_VERSION    "0.1.0"

#endif // SYSTEM_CONFIG_H

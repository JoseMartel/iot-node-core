#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

#include "esp_err.h"

/**
 * @brief Initialize the system utility module.
 * 
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t system_utils_init(void);

/**
 * @brief Log the current system health (Free RAM, Task stack, etc.)
 */
void system_utils_log_health(void);

#endif // SYSTEM_UTILS_H

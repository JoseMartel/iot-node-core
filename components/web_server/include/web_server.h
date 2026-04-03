#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "esp_err.h"

/**
 * @brief Inicializa el servidor web y registra los handlers.
 * 
 * @return esp_err_t ESP_OK en éxito.
 */
esp_err_t web_server_init(void);

/**
 * @brief Detiene el servidor web.
 */
void web_server_stop(void);

#endif // WEB_SERVER_H

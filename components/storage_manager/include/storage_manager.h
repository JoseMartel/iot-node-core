#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include "esp_err.h"
#include "system_types.h"

/**
 * @brief Inicializa el storage manager usando NVS.
 * 
 * @return esp_err_t ESP_OK en éxito.
 */
esp_err_t storage_manager_init(void);

/**
 * @brief Guarda un paquete binario en el buffer circular de NVS.
 * 
 * @param packet Estructura con los datos.
 * @return esp_err_t ESP_OK en éxito.
 */
esp_err_t storage_push_packet(const iot_packet_t* packet);

/**
 * @brief Obtiene y remueve el paquete más antiguo del buffer circular.
 * 
 * @param packet Puntero donde se copiarán los datos.
 * @return esp_err_t ESP_OK si hay datos, ESP_ERR_NOT_FOUND si está vacío.
 */
esp_err_t storage_pop_packet(iot_packet_t* packet);

/**
 * @brief Obtiene la cantidad de paquetes pendientes.
 */
uint16_t storage_get_pending_count(void);

#endif // STORAGE_MANAGER_H

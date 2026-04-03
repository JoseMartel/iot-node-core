#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "esp_err.h"
#include "system_types.h"

/**
 * @brief Inicializa los sensores.
 */
esp_err_t sensor_manager_init(void);

/**
 * @brief Lee los sensores y rellena el paquete.
 */
esp_err_t sensor_manager_read(iot_packet_t* packet);

#endif // SENSOR_MANAGER_H

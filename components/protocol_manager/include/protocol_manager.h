#ifndef PROTOCOL_MANAGER_H
#define PROTOCOL_MANAGER_H

#include "esp_err.h"
#include "system_types.h"

/**
 * @brief Envía un paquete y retorna el código de estado HTTP.
 * 
 * @param packet Estructura binaria con los datos.
 * @return int Código HTTP (200, 400, 500) o un valor negativo si hay error de conexión.
 */
int protocol_send_packet(const iot_packet_t* packet);

#endif // PROTOCOL_MANAGER_H

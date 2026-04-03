#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>

/**
 * @brief Estructura de telemetría unificada.
 * Ocupa un tamaño fijo y conocido, ideal para almacenamiento binario.
 */
typedef struct {
    uint32_t msg_id;
    uint32_t timestamp;     // Tiempo Unix si hay NTP, o uptime
    float temperature;
    float humidity;
    uint16_t battery_mv;    // Milivoltios
    uint32_t free_heap;
    uint8_t status_code;    // Bitmask de estado
} iot_packet_t;

#endif // SYSTEM_TYPES_H

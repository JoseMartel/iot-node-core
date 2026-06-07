#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "esp_err.h"
#include <time.h>
#include <stdbool.h>

/**
 * @brief Inicializa el servicio SNTP para sincronizar la hora por internet.
 */
void time_manager_init(void);

/**
 * @brief Verifica si el sistema ya tiene la hora sincronizada.
 * 
 * @return true Si la hora es válida (post-1970).
 */
bool time_manager_is_synced(void);

/**
 * @brief Obtiene el timestamp Unix actual.
 * 
 * @return uint32_t Segundos desde 1970.
 */
uint32_t time_manager_get_timestamp(void);

#endif // TIME_MANAGER_H

#ifndef DNS_SERVER_H
#define DNS_SERVER_H

/**
 * @brief Inicia un servidor DNS minimalista que redirige todas las consultas
 * a la IP del ESP32 (192.168.4.1).
 */
void dns_server_start(void);

/**
 * @brief Detiene el servidor DNS.
 */
void dns_server_stop(void);

#endif // DNS_SERVER_H

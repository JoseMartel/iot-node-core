idf_component_register(SRCS "network_manager.c"
                    INCLUDE_DIRS "include"
                    REQUIRES config_manager
                    PRIV_REQUIRES esp_wifi esp_event esp_netif)

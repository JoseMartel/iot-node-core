idf_component_register(SRCS "system_utils.c" "time_manager.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES esp_netif lwip)

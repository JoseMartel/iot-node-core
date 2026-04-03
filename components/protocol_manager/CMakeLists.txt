idf_component_register(SRCS "protocol_manager.c"
                    INCLUDE_DIRS "include"
                    REQUIRES system_utils
                    PRIV_REQUIRES esp_http_client)

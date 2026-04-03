idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES system_utils config_manager network_manager protocol_manager storage_manager sensor_manager web_server)

idf_component_register(SRCS "storage_manager.c"
                    INCLUDE_DIRS "include"
                    REQUIRES system_utils
                    PRIV_REQUIRES nvs_flash)

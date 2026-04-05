idf_component_register(SRCS "web_server.c" "dns_server.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_server config_manager storage_manager system_utils json
                    EMBED_FILES "web_assets/index.html")

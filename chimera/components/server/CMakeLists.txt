idf_component_register(SRCS "server.c"
                    INCLUDE_DIRS "include"
                        REQUIRES "sta-AP"
                        REQUIRES "esp_http_server"
                            EMBED_FILES "../../index.html")
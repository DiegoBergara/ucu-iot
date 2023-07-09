idf_component_register(SRCS "sntpServer.c"
                    REQUIRES "sta-AP" "server" "freertos" "delay"
                    INCLUDE_DIRS "include")

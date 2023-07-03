idf_component_register(SRCS "main.c" "sensor/sensor.c"
                    INCLUDE_DIRS "." "sensor"
                        REQUIRES "sta-AP"
                        REQUIRES "server")
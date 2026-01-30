idf_component_register(
    SRCS
        "Wifi_MQTT.c"
        "onenet_token.c"
        "onenet_dm.c"
        "cJSON.c"
    INCLUDE_DIRS "."
    REQUIRES
        mbedtls
        mqtt
)

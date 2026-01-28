idf_component_register(
    SRCS
        "Wifi_MQTT.c"
        "onenet_token.c"
    INCLUDE_DIRS "."
    REQUIRES
        mbedtls
        mqtt
)

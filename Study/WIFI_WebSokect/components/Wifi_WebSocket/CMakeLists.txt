idf_component_register(
    SRCS
        "Wifi_WebSocket.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_http_server MyLED spiffs spi_flash nvs_flash
)

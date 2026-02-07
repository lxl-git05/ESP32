idf_component_register(
    SRCS "WIfi_Http.c" "Wifi_Http.c"
    INCLUDE_DIRS "."
    REQUIRES
        nvs_flash
        esp_wifi
        esp_event
        esp_netif
        esp_http_server
)

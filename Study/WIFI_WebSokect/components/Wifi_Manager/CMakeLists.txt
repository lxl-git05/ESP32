idf_component_register(
    SRCS "Wifi_Manager.c"
    INCLUDE_DIRS "."
    REQUIRES
        nvs_flash
        esp_wifi
        esp_event
        esp_netif
        Wifi_MQTT   # ÒÀÀµµÄmqtt_onenet×é¼þ
)
